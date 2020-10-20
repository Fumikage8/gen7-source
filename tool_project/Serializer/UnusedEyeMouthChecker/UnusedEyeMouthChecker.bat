@ECHO OFF

SET NIJI_TOOL_DIR=R:/home/niji/tool

CALL R:\home\niji\exec\environment\macro_setting.bat

ruby UnusedEyeMouthChecker.rb

pause;