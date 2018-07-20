echo 开始拷贝运行环境
xcopy ..\..\bin ..\..\BaseGraphInterface\bin /s /y /exclude:exclude.txt >nul
echo 拷贝结束