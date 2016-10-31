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

        private System.IO.Ports.SerialPort m_serialPort;


        private bool m_bDoDisconnect = false;
        private bool m_bConnected = false;
        private Thread m_connectorThread;

        public EnConnState ConnectionState { get; private set; }

        public UCConnector()
        {
            m_serialPort = new System.IO.Ports.SerialPort();
            m_connectorThread = null;

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

        public void connect(String portName, UInt32 baudRate)
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

            m_connectorThread = new Thread(new ThreadStart(connectorthread));

            m_connectorThread.Name = "Connector_connectorthread";
            m_connectorThread.Start();

            return;
        }

        private Boolean m_bTxQueueEmpty = true;
        private String m_szTxMsg;

        public void send(String msg)
        {
            if (!m_bConnected)
            {
                return;
            }

            if (m_bTxQueueEmpty)
            {
                m_szTxMsg = msg;
                m_bTxQueueEmpty = false;
            }

            return;
        }


        public void disconnect()
        {
            m_bDoDisconnect = true;

            ConnectionState = EnConnState.DISCONNECTING;
            ConnectionStateChange?.Invoke(this, null);

            return;
        }

        private void connectorthread()
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
                        if ( (rxbyte == (Byte)':') || (rxbyte == (Byte)'\'') || (rxbyte == (Byte)'#'))
                        {
                            bRxInProgress = true;
                            rx.Clear();
                            rx.Append((Char)rxbyte);
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
                            }
                            else if (msg[0] == '\'')
                            {
                                NewDisplayData?.Invoke(this, msg);
                            }
                            else if (msg[0] == '#')
                            {
                                NewDAQData?.Invoke(this, msg);
                            }

                        }
                    }
                }

                if (!m_bTxQueueEmpty)
                {
                    Byte[] tmp = Encoding.ASCII.GetBytes(m_szTxMsg);
                    Byte[] tx = new Byte[tmp.Length + 1];
                    tmp.CopyTo(tx, 0);
                    tx[tx.Length - 1] = 10;

                    m_serialPort.Write(tx, 0, tx.Length);
                    m_bTxQueueEmpty = true;
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
