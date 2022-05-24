@echo off
::拷贝算法库到此目录
copy /Y/B Objects\Aids_Algorithm_lib.lib .\Aids_Algorithm_lib.lib

::拷贝HEX文件到此目录
copy /Y/B Objects\STM32H743VIT6_Aids_CDC.hex .\STM32H743VIT6_Aids_CDC.hex
copy /Y/B Objects\STM32H743VIT6_Aids_AUDIO.hex .\STM32H743VIT6_Aids_AUDIO.hex

::拷贝AXF文件
copy /Y/B Objects\STM32H743VIT6_Aids_APP.axf ..\DebugTool\STM32H743VIT6_Aids_APP.axf
copy /Y/B Objects\STM32H743VIT6_Aids_CDC.axf ..\DebugTool\STM32H743VIT6_Aids_CDC.axf
copy /Y/B Objects\STM32H743VIT6_Aids_AUDIO.axf ..\DebugTool\STM32H743VIT6_Aids_AUDIO.axf

echo "Copy File Ok! Exit..."
