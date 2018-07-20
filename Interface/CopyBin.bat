xcopy ..\bin .\bin /s /y 
for /R %%s in (*.iobj,*.ipdb,*.ilk) do (
del %%s
echo delete %%s
)
