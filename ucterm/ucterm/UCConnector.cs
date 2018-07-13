using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;

namespace ucterm
{
    class UCConnector
    {
        public enum EnConnState
        {
            DISCONNECTED,
            CONNECTING,
            FAILED_TO_CONNECTED,
            CONNECTED,
            BROKEN,
            TRY_REVIVE,
            SEARCH,
            DISCONNECTING
        };

        public delegate void ConnectionStateChangeHandler(object sender, EventArgs data);
        public event ConnectionStateChangeHandler ConnectionStateChange;

        public delegate void NewRespDataHandler(object sender, String data);
        public event NewRespDataHandler NewRespData;

        public delegate void NewDAQDataHandler(object sender, String data);
        public event NewDAQDataHandler NewDAQData;

        public delegate void NewDisplayDataHandler(object sender, String data);
        public event NewDisplayDataHandler NewDisplayData;

		public delegate void NewCorruptDataHandler(object sender, String data);
		public event NewCorruptDataHandler NewCorruptData;

		public delegate void AsyncSendCompletedHandler(object sender);
		public event AsyncSendCompletedHandler AsyncSendCompleted;

		private System.IO.Ports.SerialPort m_serialPort;


        private bool m_bDoDisconnect = false;
        private bool m_bConnected = false;
        private Thread m_connectorThread;
		private Thread m_sendThread;

		public EnConnState ConnectionState { get; private set; }

        public UCConnector()
        {
			m_txmsgs = new Queue<string>();

			m_serialPort = new System.IO.Ports.SerialPort();
            m_connectorThread = null;
			m_sendThread = null;

            ConnectionState = EnConnState.DISCONNECTED;

			return;
        }


        ~UCConnector()
        {
            if (m_serialPort.IsOpen)
            {
                m_serialPort.Close();
            }

            return;
        }

        public void Connect(String portName, UInt32 baudRate)
        {
            if (m_bConnected)
            {
                return;
            }

            try
            {
                m_serialPort.PortName = portName;
                m_serialPort.BaudRate = (Int32)baudRate;

                m_serialPort.DataBits = 8;
                m_serialPort.StopBits = System.IO.Ports.StopBits.One;
                m_serialPort.Parity = System.IO.Ports.Parity.None;
                m_serialPort.Handshake = System.IO.Ports.Handshake.None;

                m_serialPort.ReadTimeout = 1;
                m_serialPort.WriteTimeout = 100;

                m_serialPort.Open();
            }
            catch
            {
                System.Windows.Forms.MessageBox.Show("Fehler");

                ConnectionState = EnConnState.FAILED_TO_CONNECTED;
                ConnectionStateChange?.Invoke(this, null);

                return;
            }

            ConnectionState = EnConnState.CONNECTED;
            ConnectionStateChange?.Invoke(this, null);

            m_bConnected = true;
            m_bDoDisconnect = false;

            if (m_connectorThread != null)
            {
                if ((m_connectorThread.ThreadState == ThreadState.Unstarted)
                        || (m_connectorThread.ThreadState == ThreadState.Stopped))
                {
                    m_connectorThread = null;
                }
            }

			m_connectorThread = new Thread(new ThreadStart(Connectorthread))
			{
				Name = "Connector_connectorthread"
			};
			m_connectorThread.Start();

            return;
        }

		private Queue<String> m_txmsgs;

		public Boolean IsTxBufferEmpty { get { return (m_txmsgs.Count == 0); } }

		private uint m_rxdebit = 0;

		public Boolean IsCmdCompleted { get { return (m_rxdebit == 0); } }

		public void ClearRxDebit()
		{
			System.Threading.Monitor.Enter(m_txmsgs);
			m_rxdebit = 0;
			System.Threading.Monitor.Exit(m_txmsgs);

			return;
		}

		public void Send(String msg)
        {
            if (!m_bConnected)
            {
                return;
            }

			System.Threading.Monitor.Enter(m_txmsgs);
			m_txmsgs.Enqueue(msg);
			++m_rxdebit;
			System.Threading.Monitor.Exit(m_txmsgs);

            return;
        }

		private Boolean m_bAbortAsyncSend = false;

		public void AsyncSend(List<String> msgs, UInt32 Nreps, Boolean bWait)
		{
			if (!m_bConnected)
			{
				AsyncSendCompleted?.Invoke(this);
				return;
			}

			if (m_sendThread != null)
			{
				if ((m_sendThread.ThreadState == ThreadState.Unstarted)
						|| (m_sendThread.ThreadState == ThreadState.Stopped))
				{
					m_sendThread = null;
				}
			}

			m_bAbortAsyncSend = false;

			m_sendThread = new Thread(new ThreadStart(() => AsyncSendThread(msgs, Nreps, bWait)))
			{
				Name = "Connector_AsyncSendThread"
			};

			m_sendThread.Start();

			return;
		}

		public void AbortAsyncSend()
		{
			m_bAbortAsyncSend = true;

			return;
		}

		private void AsyncSendThread(List<String> msgs, UInt32 Nreps, Boolean bWait)
		{
			for (uint k = 0; k < Nreps; ++k)
			{
				foreach (String cmd in msgs)
				{
					if (cmd.Trim() == "")
					{
						continue;
					}

					if (m_bAbortAsyncSend)
					{
						break;
					}

					Send(cmd.Trim());

					if (bWait)
					{
						uint w = 0;

						while (!IsCmdCompleted && !m_bAbortAsyncSend)
						{
							System.Threading.Thread.Sleep(100);

							if (++w == 10)
							{
								ClearRxDebit();

								NewRespData?.Invoke(this, "[[giving up on answer]]");
								break;
							}
						}
					}
				}

				if (m_bAbortAsyncSend)
				{
					break;
				}
			}

			AsyncSendCompleted?.Invoke(this);

			return;
		}

		public void Disconnect()
        {
            m_bDoDisconnect = true;

            ConnectionState = EnConnState.DISCONNECTING;
            ConnectionStateChange?.Invoke(this, null);

            return;
        }

        private void Connectorthread()
        {
            StringBuilder rx = new StringBuilder(10000);
            Boolean bRxInProgress = false;

            while (!m_bDoDisconnect)
            {
                Byte rxbyte;

                try
                {
                    rxbyte = (Byte)m_serialPort.ReadByte();
                }
                catch (TimeoutException)
                {
                    rxbyte = 0;
                }
                catch
                {
                    ConnectionState = EnConnState.BROKEN;
                    ConnectionStateChange?.Invoke(this, null);
                    break;
                }

                if (rxbyte > 0)
                {
                    if (!bRxInProgress)
                    {
						if (rxbyte == 0x02)
                        {
                            bRxInProgress = true;
                            rx.Clear();
                        }
                    }
                    else
                    {
                        if (rxbyte != 0x03)
                        {
                            rx.Append((Char)rxbyte);
                        }
                        else
                        {
                            bRxInProgress = false;

                            String msg = rx.ToString();

                            if (msg[0] == ':')
                            {
								NewRespData?.Invoke(this, msg);

								System.Threading.Monitor.Enter(m_txmsgs);
								if (m_rxdebit > 0)
								{
									--m_rxdebit;
								}
								System.Threading.Monitor.Exit(m_txmsgs);
                            }
                            else if (msg[0] == '\'')
                            {
                                NewDisplayData?.Invoke(this, msg);
                            }
                            else if (msg[0] == '#')
                            {
                                NewDAQData?.Invoke(this, msg);
                            }
							else
							{
								NewCorruptData?.Invoke(this, msg);
							}
                        }
                    }
                }

				String txmsg = null;

				System.Threading.Monitor.Enter(m_txmsgs);
				if (m_txmsgs.Count > 0)
				{
					txmsg = m_txmsgs.Dequeue();
				}
				System.Threading.Monitor.Exit(m_txmsgs);

				if (txmsg != null)
				{
					NewRespData?.Invoke(this, txmsg);

					Byte[] tmp = Encoding.ASCII.GetBytes(txmsg);
					Byte[] tx = new Byte[tmp.Length + 1];
					tmp.CopyTo(tx, 0);
					tx[tx.Length - 1] = 10;

					m_serialPort.Write(tx, 0, tx.Length);
				}
            }

            if (m_serialPort.IsOpen)
            {
                m_serialPort.Close();
            }

            m_bConnected = false;

            ConnectionState = EnConnState.DISCONNECTED;
            ConnectionStateChange?.Invoke(this, null);

            return;
        }
    }
}
