python bin\msocsample.py enc b.xlsx c.xlsx test
python bin\msocsample.py opt c.xlsx test
python bin\msocsample.py dec c.xlsx d.xlsx test
diff b.xlsx d.xlsx
python bin\msocsample.py enc b.xlsx c.xlsx test -sc 12345 -sk 12ab
python bin\msocsample.py opt c.xlsx test
python bin\msocsample.py dec-sk c.xlsx d.xlsx 12ab
diff b.xlsx d.xlsx
