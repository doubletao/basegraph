echo 开始拷贝运行环境
xcopy ..\..\bin ..\..\BaseGraphInterface\bin /s /y /exclude:exclude.txt >nul
copy ..\..\include\Common\DataDef.h ..\..\BaseGraphInterface\include\DataDef.h >nul
copy ..\..\include\DataBase\DataTypeDef.h ..\..\BaseGraphInterface\include\DataTypeDef.h >nul
echo 拷贝结束