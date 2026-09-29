import ROOT
import time
import datetime
import sys

ROOT.gInterpreter.Declare('#include "/storage/hive/project/phy-otte/shared/analysistools/exact/dict/include/Event.h"')
ROOT.gSystem.Load("/storage/hive/project/phy-otte/shared/analysistools/exact/dict/obj/Event.o")
ROOT.gInterpreter.Declare('#include "/storage/hive/project/phy-otte/shared/analysistools/exact/dict/include/ExtractedData.h"')
ROOT.gSystem.Load("/storage/hive/project/phy-otte/shared/analysistools/exact/dict/obj/ExtractedData.o")
ROOT.gInterpreter.Declare('#include "/storage/hive/project/phy-otte/shared/analysistools/exact/dict/include/Pulse.h"')
ROOT.gSystem.Load("/storage/hive/project/phy-otte/shared/analysistools/exact/dict/obj/Pulse.o")
ROOT.gInterpreter.Declare('#include "/storage/hive/project/phy-otte/shared/analysistools/exact/dict/include/ReadConfiguration.h"')
ROOT.gSystem.Load("/storage/hive/project/phy-otte/shared/analysistools/exact/dict/obj/ReadConfiguration.o")
ROOT.gSystem.Load("libExACT.so")	

iLastPixelHLED = -1
hPixelTraceLED = None
ev = ROOT.Event()
hChg = None
hcam = None
text = None


def TCameraPlotEvent(evnum):
	global event_entries
	global tree
	p_disp.cd()
	PlotEvent(EventCounter)
	
def PlotEvent(num):
	global tree
	global p_disp
	tree.GetEntry(num)
	#p_disp.cd(3)
	#PlotTrace(128)
	#plot MUSIC
	PlotCharge()

	p_disp.cd(4)
	ROOT.gPad.Clear()	
	eventnum = "Event #: %i" % (num)
	EventNumba = ROOT.TLatex()
	EventNumba.SetNDC()
	EventNumba.SetTextSize(0.04)
	textNumba = EventNumba.DrawLatexNDC(0.1,0.95,eventnum)

	convtime = ev.GetTBTime()/100000000
	epochTime = datetime.datetime.utcfromtimestamp(convtime)
	microsec = convtime % 1000000
	timenum = "Trigger Time (UTC): " + epochTime.strftime("%Y-%m-%d %H:%M:%S"+str(microsec))
	TimeNumba = ROOT.TLatex()
	TimeNumba.SetNDC()
	TimeNumba.SetTextSize(0.04)
	ttextNumba = TimeNumba.DrawLatexNDC(0.1,0.75, timenum)
	
	ROOT.gPad.Modified()
	ROOT.gPad.Update()
		

def PlotTrace(iPix):
	global ev
	global hPixelTraceLED
	if hPixelTraceLED == None:
		hPixelTraceLED = ROOT.TH1F("hPixelTraceLED","Pixel Trace",500,-0.5,499.5)
		hPixelTraceLED.SetStats(0)
		hPixelTraceLED.GetXaxis().SetTitle("ADC sample")
		hPixelTraceLED.GetYaxis().SetTitle("ADC counts")

	hPixelTraceLED.Reset()
	title = "Trace of Pixel {}".format(iPix)
	hPixelTraceLED.SetTitle(title)
	trace = ev.GetSignalValue(iPix)
	for k in range(512):
		hPixelTraceLED.SetBinContent(k+1, trace[k])
	
	hPixelTraceLED.Draw()
	ROOT.gPad.Modified()
	ROOT.gPad.Update()
		
def PlotCharge():
	global hChg
	global hcam
	print("Event #" + str(EventCounter))
	if hChg == None:
		p_disp.cd(1)
		ROOT.gInterpreter.Declare('''
void PixelClickedWrapper() {
	 TPython::Exec("PixelClicked()");
}
 ''')
		ROOT.gPad.AddExec("ev","PixelClickedWrapper()")
		hcam = ROOT.TH2F("hcam","",16,-0.5,15.5,16,-0.5,15.5)
		hcam.SetStats(0)
		hcam.Draw("colz")
	
		line1 = ROOT.TLine(-.5, -0.5, 3.5, 1.5)
		line1.Draw()
		
		hChg = ROOT.TH1F("hChg","Charge Distribution",410, 0, 4096)
		hChg.SetStats(0)
		hChg.GetXaxis().SetTitle("Charge [ADC]")
		hChg.GetYaxis().SetTitle("No. of Pixels")
	
	hcam.Reset()
	hChg.Reset()
	extractedDataHLED = [0]*256
	for i in range(256):
		extractedDataHLED[i] = ROOT.ExtractedData()
		
	for j in range(256):
		pulse = ROOT.Pulse(ev.GetSignalValue(j))
		extractedDataHLED[j].SetAmplitude(pulse.GetAmplitude())
		nx, ny = FindBin(j)
		hcam.SetBinContent(nx+1, ny+1, extractedDataHLED[j].GetAmplitude()) 
		hChg.Fill(extractedDataHLED[j].GetAmplitude())
	
	p_disp.cd(2)	
	hChg.Draw()
	
	ROOT.gPad.Modified()
	ROOT.gPad.Update()
def FindBin(Pixel_ID):
	SIAB_Number = Pixel_ID / 16
	SIAB_Pixel_Number = Pixel_ID % 16
	SIAB_Pixel_Row = SIAB_Pixel_Number / 4
	SIAB_Pixel_Col = SIAB_Pixel_Number % 4
	row = SIAB_Number / 4 * 4 + SIAB_Pixel_Row
	col = SIAB_Number % 4 * 4 + SIAB_Pixel_Col
	return row, col

def FindPixel(row, col):
	SIAB_Row = row/4
	SIAB_Col = col/4
	SIAB_Number = SIAB_Row * 4 + SIAB_Col
	
	SIAB_Pixel_Row = SIAB_Row % 4
	SIAB_Pixel_Col = SIAB_Col % 4
	SIAB_Pixel_Number = SIAB_Pixel_Row * 4 + SIAB_Pixel_Col
	Pixel_Number = SIAB_Number * 16 + SIAB_Pixel_Number
	return Pixel_Number

def PixelClicked():
	
	event = ROOT.gPad.GetEvent()
	o = ROOT.gPad.GetSelected()
	if not o:
		return
	if not (o.InheritsFrom("TH2")):
		return 

	px = ROOT.gPad.GetEventX()
	py = ROOT.gPad.GetEventY()
	xx = ROOT.gPad.AbsPixeltoX(px)
	yy = ROOT.gPad.AbsPixeltoY(py)
	x = 0.5+ROOT.gPad.PadtoX(xx)
	y = 0.5+ROOT.gPad.PadtoY(yy)
	pix = FindPixel(int(x), int(y))
	if (pix != iLastPixelHLED and event == 1):
		iLastPix = -1
		ShowInfoAtCursor(int(x), int(y))


def ShowInfoAtCursor(x,y):
	global text
	MUSIC_column = x/2
	MUSIC_row = y/4
	MUSIC_ID = MUSIC_column+MUSIC_row*8
	MUSIC_Channel = y%4+4*(x%2)
	PixID = MUSIC_row*4*16+MUSIC_column*8+MUSIC_Channel
	
	statusline = "Event #: %i  MUSIC_ID: %i    MUSIC_Channel: %i    Pixel ID: %i" % (EventCounter, MUSIC_ID, MUSIC_Channel, PixID)
	if(text!=None):
		text.Delete()
	T1 = ROOT.TLatex()
	T1.SetNDC()
	T1.SetTextAlign(13)
	T1.SetTextSize(0.04)
	text = T1.DrawLatexNDC(0,0.95,statusline)
	ROOT.gPad.Modified()
	ROOT.gPad.Update()
	p_disp.cd(3)
	PlotTrace(PixID)	

def SpaceClickedPy():
	global EventCounter
	event = ROOT.gPad.GetEvent()
	if event == 24:
		if EventCounter < tree.GetEntries():
			EventCounter += 1
			TCameraPlotEvent(EventCounter)
		else:
			EventCounter = 0
		
if __name__ == "__main__":	
	file_path = sys.argv[1]
	root_file = ROOT.TFile.Open(file_path)
	tree_name = sys.argv[2]
	tree = root_file.Get(tree_name)
	tree.SetBranchAddress("Events", ROOT.AddressOf(ev))
	EventCounter = 0
	preEventCounter = int(sys.argv[3])	
	if 0 <= preEventCounter < tree.GetEntries():
		EventCounter = preEventCounter
	else:
		print("Event not Found")
	p_disp = ROOT.TCanvas("Display", "CameraPlot", 950, 1000)
	p_disp.Divide(2,2)
	print("To swap events press spacebar to move through event click left mouse")	
	ROOT.gInterpreter.Declare('''
void SpaceClicked() {
	 TPython::Exec("SpaceClickedPy()");
}
 ''')
	ROOT.gPad.AddExec("ev2","SpaceClicked()")
	TCameraPlotEvent(EventCounter)
	ROOT.gApplication.Run()
