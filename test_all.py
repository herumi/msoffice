import os, sys, subprocess

EXE='bin/msoffice-cryptd.exe'
INP='data'
OUT='out'

try:
	os.mkdir(OUT)
except:
	pass

def verify(name):
	[base,suf] = name.split('.')
	print base, suf
	enc1 = INP + '/' + base + '.' + suf
	dec1 = OUT + '/' + base + '.' + suf
	enc2 = OUT + '/' + base + '_e.' + suf
	dec2 = OUT + '/' + base + '_d.' + suf
	ps1='test'
	ps2='test2'
	subprocess.check_call([EXE, "-d", "-p", ps1, enc1, dec1])
	subprocess.check_call([EXE, "-e", "-p", ps2, dec1, enc2])
	subprocess.check_call([EXE, "-d", "-p", ps2, enc2, dec2])
	data1 = open(dec1, 'rb').read()
	data2 = open(dec2, 'rb').read()
	print "size", len(data1)
	if data1 != data2:
		print "bad decode:not equal", dec1, dec2
		exit(1)

def main():

	if len(sys.argv) == 1:
		files = [file for file in os.listdir(INP) if file.endswith('x')]
	else:
		files = [sys.argv[1].replace('\\', '/').split('/')[-1]]

	try:
		for file in files:
			verify(file)
	except Exception as e:
		print e
		exit(1)

if __name__ == '__main__':
    main()
