
Setup STLink.py als External Tool in System Workbench for STM32:
	Name              : STM flashen
	Executable        : C:\Windows\py.exe
	Working directory : ${workspace_loc:/pses_ucboard/}
	Arguments         : stlink.py --hexfile="${workspace_loc}\${project_name}\${config_name:${project_name}}\${project_name}.bin"


	
Setup STLink.py als User-defined-Tool in EmBitz, so dass Controller ohne Debugging geflasht werden kann:

Einstellung unter "Tools -> Configure Tools..."

STM flashen:
	Name              : STM flashen
	Executable        : py
	Parameters        : stlink.py --hexfile="${TARGET_OUTPUT_DIR}/${TARGET_OUTPUT_BASENAME}.hex" --waitaftererror
	Working directory : ${PROJECT_DIR}
	
STM reseten:
	Name              : STM reseten
	Executable        : py
	Parameters        : stlink.py --reset --waitaftererror
	Working directory : ${PROJECT_DIR}
	
	
(Damit dies funktioniert, muss auf jeden Fall beim Build ein hex-File erzeugt werden.
Dies ist in diesem Projekt für beide Targets (Debug und Release) so eingestellt.)
