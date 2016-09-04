
' decrypt Excel file with password

Option Explicit
Dim g_fs
set g_fs = wscript.createObject("scripting.fileSystemObject")

ParseAndRun wscript.arguments
wscript.quit

sub ParseAndRun(args)
	Dim pass, inName, outName
	if args.unnamed.Count <> 3 then
		wscript.echo "decrypt-xls.vbs pass inName outFileName"
		wscript.quit
	end if
	pass = args.unnamed(0)
	inName = g_fs.getAbsolutePathName(args.unnamed(1))
	outName = g_fs.getAbsolutePathName(args.unnamed(2))

	Dim suf
	suf = lcase(g_fs.getExtensionName(inName))
	if suf <> "xls" and suf <> "xlsx" then
		wscript.echo "bad suffix=" & suf
		wscript.quit
	end if

	decryptExcel inName, outName, pass
end sub

sub decryptExcel(inName, outName, pass)
	on error resume next

	Dim app
	set app = createObject("Excel.application")
	app.Application.DisplayAlerts = False
	app.Visible = False

	Dim wb
	set wb = app.Workbooks.open(inName, , , , pass)
	wb.SaveAs outName, , ""
	app.quit
	if err.number <> 0 then
		wscript.echo "ERR:" & err.number & " desc:" & err.description
	end if
end sub
