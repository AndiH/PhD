/**
 *  @file common.cpp
 *  @Author Andreas Herten (a.herten@gmail.com)
 *  @date 2014
 *  @brief Handy little functions for ROOT
 *  
 * This file holds all the common functions I wrote while working on stuff. 
 * Usually ROOT, sometimes C++. Use at own risk, participation welcome. 
 * -Andreas
 */

#include "TROOT.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TTree.h"

#include "TCanvas.h"
#include "TPad.h"

#include "TLegend.h"
#include "TPaveText.h"
#include "TPaveStats.h"
#include "TPaletteAxis.h"

#include "TH1.h"
#include "TH2.h"
#include "THStack.h"
#include "TF1.h"
#include "tableau_colors.cpp"

namespace andi {  // everything is in a name space to structure it better
	// some constants
	double crossSig = 0.1; // mu barn
	double crossBkg = 45737; // mu barn
	double brSig = 0.0931;
	/**
	 * @brief Set my custom ROOT canvas style
	 * @details Gets rid of the margins and sets some other stuff
	 */
	void setCustomStyle(bool showStats = true) {
		TStyle* customStyle = new TStyle("customStyle", "customStyle"); 
		// TStyle * plainStyle = gROOT->GetStyle("Plain");
		// TStyle * customStyle = new TStyle(*plainStyle); 
		// customStyle->SetName("customStyle");

		// Normalize
		customStyle->SetOptStat(0);  // Don't display statbox
		customStyle->SetOptDate(0);  // Don't display any date anywhere
		customStyle->SetOptTitle(0);  // Don't display title on top of canvas
		customStyle->SetOptFit(0);  // Don't display fit information in statbox (http://root.cern.ch/root/html/TStyle.html#TStyle:SetOptFit)


		customStyle->SetPalette(1,0);  // avoid horrible default color scheme 
		customStyle->SetCanvasColor(0); 

		customStyle->SetDrawBorder(0);  // No border
		customStyle->SetCanvasBorderMode(0); // ?
		customStyle->SetFrameBorderMode(0);
		customStyle->SetPadBorderMode(0);
		customStyle->SetLegendBorderSize(0);
		
		// Change axis
		customStyle->SetLabelSize(0.04, "xyz");  // size of labels on all three axes
		customStyle->SetLabelFont(62, "xyz");  // font style for all three axes
		
		customStyle->SetTitleSize(0.045, "xyz");  // Axis title size
		customStyle->SetTitleOffset(0.8, "xyz");  // Axis title offset
		customStyle->SetTitleOffset(1.0, "y");  // A bit more offset for z axis (right)
		customStyle->SetTitleFont(62, "xyz");  // font style for all three axes

		customStyle->SetPadTopMargin(0.07);
		customStyle->SetPadRightMargin(0.04);

		// Date
		// This prints a white date in the lower left corner of the picture - not visible (but copyable)
		// http://root.cern.ch/root/html/TStyle.html#TStyle:SetOptDate
		customStyle->SetOptDate(20);
		// TColor * lightGray = new TColor(2000, 0.95, 0.95, 0.95); // Defines new color
		// int lightGray = TColor::GetColor("#0000ff"); // Alternatively defines new color / gets existing one
		// customStyle->GetAttDate()->SetTextColor(19); // 19 = off white
		customStyle->GetAttDate()->SetTextColor(kWhite); // 19 = off white
		
		// Title
		customStyle->SetTitleBorderSize(1);
		customStyle->SetTitleFillColor(kWhite);
		customStyle->SetTitleFontSize(0.03);
		customStyle->SetTitleX(0.96-0.26);
		customStyle->SetTitleY(0.98);
		customStyle->SetTitleW(0.26);
		customStyle->SetTitleH(0.98-0.92);

		// Statbox
		// http://root.cern.ch/root/html/TStyle.html#TStyle:SetOptStat@1
		customStyle->SetOptStat("e"); 
		if (showStats) customStyle->SetOptStat("RouMe"); 
		customStyle->SetStatColor(kWhite);
		customStyle->SetStatBorderSize(1);
		customStyle->SetStatX(1-0.04);
		// customStyle->SetStatY(1-0.02);
		customStyle->SetStatY(1-0.07);
		customStyle->SetStatFontSize(0.032);
		customStyle->SetStatH(0.14);
		customStyle->SetStatW(0.26);
		customStyle->SetStatTextColor(kGray+3);
		
		gROOT->SetStyle("customStyle");
	}

	/**
	 * @brief Abstract method for moving any statbox left
	 * @details Is used by moveStatBoxLeft and moveZAxisLeft to move stuff left
	 * 
	 * @param pave Pointer to TPave to be moved
	 * @param moveToLeft Value, the box should be moved left. In NDC.
	 */
	void moveTPaveLeft(TPave * pave, float moveToLeft = 0) {
		// gPad->Update();
		float oldValue1 = pave->GetX1NDC();
		float oldValue2 = pave->GetX2NDC();
		pave->SetX1NDC(oldValue1 - moveToLeft);
		pave->SetX2NDC(oldValue2 - moveToLeft);
		// gPad->Modified();
		// gPad->Update();
	}
	/**
	 * @brief Moves a statbox to the left
	 * 
	 * @param hist Pointer to histogram with statbox inside
	 * @param moveToLeft Percentage of how much to move it left. Negative numbers to move it right.
	 */
	void moveStatBoxLeft(TH1 * hist, float moveToLeft = 0) {
		TPaveStats * stats = (TPaveStats *)hist->GetListOfFunctions()->FindObject("stats");
		if (stats != NULL) moveTPaveLeft((TPave *)stats, moveToLeft);
	}
	/**
	 * @brief Moves a Z axis left
	 * @details Ever got the problem that your z axis is too far to the right so that the numbers are not displayed correctly? Fear not, for there is this method.
	 * 
	 * @param hist Point to histogram containing Z axis. It's a TH2 as only those should have a z axis
	 * @param moveToLeft The amount to be moved left. In NDC.
	 */
	void moveZAxisLeft(TH2 * hist, float moveToLeft = 0) {
		TPaletteAxis * axis = (TPaletteAxis *)hist->GetListOfFunctions()->FindObject("palette");
		moveTPaveLeft((TPave *)axis, moveToLeft);
	}
	void moveTPaveDown(TPave * pave, float moveDown = 0) {
		float oldValue1 = pave->GetY1NDC();
		float oldValue2 = pave->GetY2NDC();
		pave->SetY1NDC(oldValue1 - moveDown);
		pave->SetY2NDC(oldValue2 - moveDown);
	}
	/**
	 * @brief Moves a statbox down
	 * 
	 * @param hist Pointer to histogram with statbox inside
	 * @param moveDown Percentage of how much to move the statbox down. Negative values to move it up.
	 */
	void moveStatBoxDown(TH1 * hist, float moveDown = 0) {
		TPaveStats * stats = (TPaveStats *)hist->GetListOfFunctions()->FindObject("stats");
		moveTPaveDown((TPave*) stats, moveDown);
		// float oldValue1 = stats->GetY1NDC();
		// float oldValue2 = stats->GetY2NDC();
		// stats->SetY1NDC(oldValue1 - moveDown);
		// stats->SetY2NDC(oldValue2 - moveDown);
	}
	/**
	 * @brief Abstract class for shrinking boxes
	 * @details Shrinks boxes by manipulating the lower left corner. Is used by shrinkStatBox.
	 * 
	 * @param box A TPave box.
	 * @param xShrinkFactor The shrinkage factor.
	 */
	void shrinkBox(TPave * box, float xShrinkFactor) {
		float length = box->GetX2NDC() - box->GetX1NDC();
		float height = box->GetY2NDC() - box->GetY1NDC();
		float ratio = height / length;
		float newLength = length * xShrinkFactor;
		float newHeight = ratio * newLength;

		float newY1 = box->GetY2NDC() - newHeight;
		float newX1 = box->GetX2NDC() - newLength;
		box->SetY1NDC(newY1);
		box->SetX1NDC(newX1);
	}
	/**
	 * @brief Shrinks a statbox, keeping its aspect ratio. length_new = factor * length_old.
	 * @details Shrinks both the width as well as the height to factor*oldvalue. So, e.g., use 0.8 to shrink your statbox to a 80% side length in relation to the original side length.
	 * 
	 * @param hist Histogram containing the statbox
	 * @param xShrinkFactor Factor of shrinkage: length_new = factor * length_old
	 */
	void shrinkStatBox(TH1 * hist, float xShrinkFactor) {
		TPaveStats * stats = (TPaveStats *)hist->GetListOfFunctions()->FindObject("stats");
		shrinkBox(stats, xShrinkFactor);

	}
	/**
	 * @brief A box with the histogram's title
	 * @details Puts the title of a histogram into a box, formats it and returns it
	 * 
	 * @param hist Pointer to histogram with title info inside
	 * @param rightBorder Right border of where the box should end
	 * 
	 * @return pointer to a TPaveText
	 */
	TPaveText * makePadTitle(TH1 * hist, double rightBorder = 0.96) {
		// canvas->SetTopMargin(0.075);
		TPaveText *pt = new TPaveText(0.16, 0.932, rightBorder, 0.99, "NDC");
		TString histName(hist->GetName()), histTitle(hist->GetTitle());
		pt->SetName("title" + histName);
		pt->SetBorderSize(1);
		pt->SetFillColor(0);
		pt->SetTextSize(0.04);
		pt->SetBorderSize(0);
		pt->AddText(histTitle);
		return pt;
	}
	/**
	 * @brief TObject version of the last one
	 **/
	TPaveText * makePadTitle(TObject * obj, double rightBorder = 0.96) {
		// canvas->SetTopMargin(0.075);
		TPaveText *pt = new TPaveText(0.16, 0.932, rightBorder, 0.99, "NDC");
		TString objName(obj->GetName()), objTitle(obj->GetTitle());
		pt->SetName("title" + objName);
		pt->SetBorderSize(1);
		pt->SetFillColor(0);
		pt->SetTextSize(0.04);
		pt->SetBorderSize(0);
		pt->AddText(objTitle);
		return pt;
	}

	/**
	 * @brief Draws a histogram's title at default position
	 * 
	 * @param hist Pointer to histogram (1D)
	 */
	void makePadTitleAndDraw(TH1 * hist) {
		TPaveText *pt = makePadTitle(hist);
		pt->Draw();
	}
	void makePadTitleAndDraw(TObject * obj) {
		TPaveText *pt = makePadTitle(obj);
		pt->Draw();
	}
	/**
	 * @brief Draws a histogram's title at default position
	 * @details Draws title and also moves the right pad's border a bit to the left to make space for z axis
	 * 
	 * @param hist Pointer to histogram (2D)
	 * @param canvas Pointer to canvas where it's drawn upon, needed for margin movement
	 */
	void makePadTitleAndDraw(TH2 * hist, TPad * pad) {  // You probably need to cast your pad to (TPad *), because they usually return a more abstract TVirtualPad
		pad->SetRightMargin(0.08);
		gPad->Update();
		TPaveText * pt = makePadTitle((TH1D *)hist, 1 - 0.08);
		TPaletteAxis * palette = (TPaletteAxis*)hist->GetListOfFunctions()->FindObject("palette");
		palette->SetX2NDC(0.95);
		// palette->SetLabelSize(0.03);
		moveStatBoxLeft(hist, 0.04);

		pt->Draw();
	}
	/**
	 * @brief Draws a histogram's title at default position and moves statbox and z axis for LARGE datasets (= long numbers on Z)
	 * @details Draws title and moves pad's right margin, the statbox and the z axis to the left. For large datasets, where the numbers on z have four (five?) digits.
	 * 
	 * @param hist Pointer to histogram (2D)
	 * @param canvas Pointer to canvas where it's drawn upon, needed for margin movement (probably something like (TPad*)c1->GetPad(0))
	 */
	void makePadTitleAndDrawLarge(TH2 * hist, TPad * pad) {
		pad->SetRightMargin(0.15);
		gPad->Update();
		andi::makePadTitle((TH1 *)hist, 1 - 0.1)->Draw();
		andi::moveStatBoxLeft((TH1 *)hist, 0.11);
		andi::moveZAxisLeft(hist, 0.10);
		andi::shrinkStatBox((TH1*)hist, 0.8);
		gPad->Update();
	}
	/**
	 * @brief Saves a canvas in three vector image formats plus root file
	 * @details Creates four files for a given canvas: SVG, PDF, EPS - and ROOT.
	 * 
	 * @param canvas Pointer to canvas as it should be saved (everything drawn already)
	 * @param name The file name with which the files are saved
	 */
	void saveCanvas_allFileNames(TCanvas * canvas, TString name) {
		canvas->SaveAs(name + ".pdf");
		canvas->SaveAs(name + ".eps");
		canvas->SaveAs(name + ".svg");
		canvas->SaveAs(name + ".root");
	}
	/**
	 * @brief Saves a canvas in some file format at right locaiton
	 * @details Will, in default call, save every canvas in four file extensions in img/BASENAME/FILENAME.{svg,pdf,eps,root}
	 * 
	 * @param canvas The canvas as it should be saved
	 * @param macroname A basenme, e.g. your macro name, to sort the images into folder
	 * @param prefixImg Will create a img subdirectory in the current directory to put all the images in, if true.
	 */
	void saveCanvas(TCanvas * canvas, TString macroname, bool prefixImg = true) {
		TString filename = canvas->GetTitle();
		filename.ReplaceAll(" ", "_");
		macroname.ReplaceAll(" ", "_");
		TString basedir = macroname + "/";
		if (prefixImg) basedir = "img/" + basedir;
		gSystem->mkdir(basedir, kTRUE);
		saveCanvas_allFileNames(canvas, basedir + filename);
	}
	/**
	 * @brief Saves a canvas, no subdirectories
	 */
	void saveCanvasFlat(TCanvas * canvas, TString macroPrefix = "") {
		TString filename = canvas->GetTitle();
		filename.ReplaceAll(" ", "_");
		if (macroPrefix != "") filename = macroPrefix + "--" + filename;
		saveCanvas_allFileNames(canvas, filename);
	}
	/**
	 * @brief Creates a legend at my favorite position, formats it
	 * @details Just a wrapper around the TLegend constructor to already have it configured with text size, background color etc.
	 * 
	 * @param x1 Start point of box on x axis
	 * @param y1 Start point of box on y axis
	 * @param x2 End point of box on x axis
	 * @param y2 End point of box on y axis
	 * @return Pointer to the legend, ready to be filled
	 */
	TLegend * plainLegend(float x1 = 0.7, float y1 = 0.62, float x2 = 0.96, float y2 = 0.695) {
		TLegend * legend = new TLegend(x1, y1, x2, y2);
		legend->SetFillColor(kWhite);
		legend->SetTextSize(0.04);
		legend->SetEntrySeparation(0.1);
		legend->SetBorderSize(1);
		return legend;
	}
	/**
	 * @brief Converts an array of histograms into a THStack
	 * @details Gives the stack proper name, tile, and axes on the way, so it can be manipulated as a usual ROOT object (because usually, it can't, as the stuff is only defined after drawing)
	 * 
	 * @param histos TObjArray with histograms. Take care of the order!
	 * @param stackAddOption Optional string which is used when stack->Add(hist, option)
	 * @return THStack of histograms
	 */
	THStack * histogramsToStack(TObjArray * histos, TString stackAddOption = "") {
		THStack * myStack = new THStack();
		int nHistograms = histos->GetEntries();
		TH1D * firstHist;
		for (int i = 0; i < nHistograms; i++) {
			myStack->Add((TH1D*)(histos->At(i)), stackAddOption);
			if (i == 0) firstHist = (TH1D*)(histos->At(0));
		}
		TString name(firstHist->GetName());
		name.Remove(0, 1);
		name.Prepend("st");
		myStack->SetName(name);
		myStack->SetTitle(firstHist->GetTitle());
		myStack->Draw("goff");
		myStack->GetXaxis()->SetTitle(firstHist->GetXaxis()->GetTitle());
		myStack->GetYaxis()->SetTitle(firstHist->GetYaxis()->GetTitle());

		return myStack;
	}

	/**
	 * @brief Creates a legend for stack
	 * @details Make sure the histograms in the stack have proper and short titles, as those are used for the legend
	 * 
	 * @param stack A stack of histograms
	 * @param header The header of the legend
	 * @param height y lower position of box
	 * @param width x lower position of box
	 * @param y2 y upper position of box
	 * 
	 * @return A legend. Dont' forget to Draw()
	 */
	TLegend * stackLegend(THStack * stack, TString header = "Histograms", double height = 0.75, double width = 0.7, double y2 = 0.93) {
		TLegend * tempLegend = andi::plainLegend(width, height, 0.96, y2);
		tempLegend->SetHeader(header);
		int nHists = stack->GetHists()->GetEntries();
		for (int i = 0; i < nHists; i++) {
			tempLegend->AddEntry(stack->GetHists()->At(i), stack->GetHists()->At(i)->GetTitle(), "LPF");
		}
		tempLegend->SetTextFont(62);
		tempLegend->SetTextSize(0.038);  // empirically found

		return tempLegend;
	}

	/**
	 * @brief Fits a gaussian to a histogram
	 * @details Fits a gaussian to the whole range of a histogram, formats the function and returns it. If verbose, it also prints out some information on it
	 * 
	 * @param hist Pointer to the histogram to fit the gaussian on
	 * @param verbose If true, some output of the guassian fit is shown
	 * 
	 * @return Pointer to the function holding the fit, readily formatted
	 */
	TF1 * gaussFit(TH1 * hist, bool verbose = false) {
		gStyle->SetOptFit(1);
		TFitResultPtr r = hist->Fit("gaus","QS0");
		TF1 * myfunc = hist->GetFunction("gaus");
		if (verbose) {
			std::cout << "Gauss fit to " << hist->GetTitle() << " (" << hist->GetName() << ")" << std::endl;
			std::cout << "  X^2 / NDF = " << myfunc->GetChisquare() << "/" << myfunc->GetNDF() << " = " << myfunc->GetChisquare()/myfunc->GetNDF() << std::endl;
			std::cout << "  mean = " << myfunc->GetParameter(1) << " pm " << myfunc->GetParError(1) << std::endl;
			std::cout << "  sigma = " << myfunc->GetParameter(2) << " pm " << myfunc->GetParError(1) << std::endl;
		}		
		myfunc->SetLineColor(hist->GetLineColor());
		myfunc->SetLineWidth(hist->GetLineWidth());
		myfunc->SetLineStyle(2);
		return myfunc;
	}
	/**
	 * @brief Function for fitting a sum of two Gaussians (not coupled)
	 * @details Fits the sum of two Gaussian distributions on a histogram. Can try to determine the ranges automatically by pre-fitting two individual Gaussians in, each, +- 10 percent of the center (inner), and +- 80 percent of the center (outer). Or, ranges of the inner and outer fits can be given. There's a dedicated function to specify the parameters of the double Gaussian fit manually, which is also called by this function.
	 * 
	 * @param hist Histogram with the data to fit to
	 * @param verbose Print fitting parameters
	 * @param useAutoRange Defined the ranges of the pre-fits automatically to be 10% and 80% around zero
	 * @param innerRangeMax The positive half of the range of the inner Gauss pre-fit
	 * @param outerRange The positive half of the range of the outer Gauss pre-fit. For non-zero-centered histograms, please use the manual function
	 * @return A TF1 with a double Gaussian. First three parameters are the parameters of the inner Gaussian, second set of three parameters are those of the outer.
	 */
	TF1 * doubleGaussFit(TH1 * hist, bool verbose = false, bool useAutoRange = true, double innerRangeMax = 0.05, double outerRangeMax = 0.3) {
		Double_t parameters[6] = {0, 0, 0, 0, 0, 0};
		gStyle->SetOptFit(1);
		if (useAutoRange) {
			innerRangeMax = hist->GetXaxis()->GetXmax() / 10;
			outerRangeMax = innerRangeMax * 8;
		}
		TF1 * fitPre1 = new TF1("fitPre1", "gaus", -innerRangeMax, innerRangeMax);
		hist->Fit(fitPre1, "Q0R");
		fitPre1->GetParameters(&parameters[0]);
// std::cout << parameters[0] << ", " << parameters[3] << std::endl;
		TF1 * fitPre2 = new TF1("fitPre2", "gaus", -outerRangeMax, outerRangeMax);
		hist->Fit(fitPre2, "Q0R");
		fitPre2->GetParameters(&parameters[3]);
		// startParameters = &parameters;
		return doubleGaussFit(hist, verbose, -outerRangeMax, outerRangeMax, parameters);

	}
	/**
	 * @brief Does the as doubleGaussFit but does not expected 0-centered distribution
	 * @details Instead of choosing the range symmetrically around zero, the histogram's main is retrieved as the central value
	 */
	TF1 * doubleGaussFitNonZero(TH1 * hist, bool verbose = false, bool useAutoRange = true, double innerRangeMax = 0.05, double outerRangeMax = 0.3) {
		double centralValue = hist->GetMean();
		Double_t parameters[6] = {0, 0, 0, 0, 0, 0};
		gStyle->SetOptFit(1);
		if (useAutoRange) {
			innerRangeMax = fabs(centralValue - hist->GetXaxis()->GetXmax()) / 10;
			outerRangeMax = fabs(centralValue - hist->GetXaxis()->GetXmax()) / 10 * 8;
		}
		// std::cout << "centralValue: " << centralValue << std::endl;
		// std::cout << "innerRangeMax: " << innerRangeMax << std::endl;
		// std::cout << "outerRangeMax: " << outerRangeMax << std::endl;
		TF1 * fitPre1 = new TF1("fitPre1", "gaus", centralValue - innerRangeMax, centralValue + innerRangeMax);
		hist->Fit(fitPre1, "Q0R");
		fitPre1->GetParameters(&parameters[0]);
		TF1 * fitPre2 = new TF1("fitPre2", "gaus", centralValue - outerRangeMax, centralValue + outerRangeMax);
		hist->Fit(fitPre2, "Q0R");
		fitPre2->GetParameters(&parameters[3]);
		// startParameters = &parameters;
		return doubleGaussFit(hist, verbose, centralValue - outerRangeMax, centralValue + outerRangeMax, parameters);

	}
	/**
	 * @brief Function for fitting a sum of two Gaussians (not coupled), providing the fit parameters
	 * @details The manual version to the other function. Expects a double pointer as last argument, pointing to six pre-fit parameters from where to start the fitting minimization.
	 * 
	 * @param hist Histogram with data
	 * @param verbose Print fit information
	 * @param outerRange The range of the double Gaussian histogram to fit stuff
	 * @param startParameters An array of six values from where to start the minimization
	 * @return Pointer to TF1 with two Gaussians. See description of other function.
	 */
	TF1 * doubleGaussFit(TH1 * hist, bool verbose, double rangeStart, double rangeEnd, double * startParameters) {
		TF1 * fitProper = new TF1("fitProper", "gaus(0)+gaus(3)", rangeStart, rangeEnd);
		fitProper->SetParameters(startParameters);
		fitProper->SetParName(0, "Const (inner)");
		fitProper->SetParName(1, "Mean (inner)");
		fitProper->SetParName(2, "Sigma (inner)");
		fitProper->SetParName(3, "Const (outer)");
		fitProper->SetParName(4, "Mean (outer)");
		fitProper->SetParName(5, "Sigma (outer)");

		hist->Fit(fitProper, "Q0R");
		if (verbose) {
			std::cout << "Gauss fit to " << hist->GetTitle() << " (" << hist->GetName() << ")" << std::endl;
			std::cout << "  X^2 / NDF = " << fitProper->GetChisquare() << "/" << fitProper->GetNDF() << " = " << fitProper->GetChisquare()/fitProper->GetNDF() << std::endl;
			std::cout << "  mean (inner) = " << fitProper->GetParameter(1) << " pm " << fitProper->GetParError(1) << std::endl;
			std::cout << "  sigma (inner) = " << fitProper->GetParameter(2) << " pm " << fitProper->GetParError(1) << std::endl;
			std::cout << "  mean (outer) = " << fitProper->GetParameter(4) << " pm " << fitProper->GetParError(4) << std::endl;
			std::cout << "  sigma (outer) = " << fitProper->GetParameter(5) << " pm " << fitProper->GetParError(5) << std::endl;
		}	

		fitProper->SetLineColor(hist->GetLineColor());
		fitProper->SetLineWidth(hist->GetLineWidth());
		fitProper->SetLineStyle(2);

		return fitProper;
	}
	/**
	 * @brief Helper function for double Gaussian function. Converts the double Gaussian into the two single Gaussians it is built from.
	 * @details 
	 * 
	 * @param funcGaus A pointer to a double Gaussian function
	 * @return A pair of pointers to the functions, the Gaussian is built from.
	 */
	std::pair<TF1 *, TF1 * > doubleGaussToTwoGauss(TF1* funcGaus) {
		Double_t parameters[6] = {-1, -1, -1, -1, -1, -1};
		funcGaus->GetParameters(&parameters[0]);
		double rangeStart, rangeEnd;
		funcGaus->GetRange(rangeStart, rangeEnd);

		TF1 * firstGaus = new TF1("firstGaus", "gaus", rangeStart, rangeEnd);
		firstGaus->SetParameters(parameters[0], parameters[1], parameters[2]);
		TF1 * secondGaus = new TF1("secondGaus", "gaus", rangeStart, rangeEnd);
		secondGaus->SetParameters(parameters[3], parameters[4], parameters[5]);

		return std::make_pair(firstGaus, secondGaus);
	}
	/**
	 * @brief Draws a (1D) histogram onto a canvas, makes the title and saves it (optionally)
	 * @details This few lines is the overhead I'm using all day, so I made this function. The canvas name is chosen as per the histogram name, with my convention (histograms start with a h, the according canvas is the same name, but with a c in front). There are some optional arguments
	 * 
	 * @param h Pointer to 1D histogram
	 * @param filename The title of the canvas, also the name the images will be saved with
	 * @param basename The basename for andi::saveCanvas, a prefix for all the images
	 * @param save Should files be saved? Slows everything down, hence an option
	 * @param fit Should there be a Gaussian fit?
	 * @return TCanvas point. You dont need to use it, but if you want, whoops, here it is.
	 */
	TCanvas * createCanvasDrawAndSave(TH1 * h, TString filename, TString basename, bool save = true, int fit = 0) {
		TString canvasName = h->GetName();
		canvasName.Remove(0, 1);
		canvasName.Prepend("c");
		TCanvas * c = new TCanvas(canvasName, filename, 0, 0, 800, 500);
		h->Draw("HIST");
		if (fit == 1) {
			TF1 * gf = gaussFit(h, true);
			gf->Draw("SAME");
		} else if (fit == 2) {
			TF1 * gf = doubleGaussFitNonZero(h, true);
			gf->Draw("SAME");
		}
		andi::makePadTitleAndDraw(h);
		if (fit == 2) {
			shrinkStatBox(h, 1.3);
		}
		if (save) andi::saveCanvas(c, basename);
		return c;
	}
	/**
	 * @brief See andi::createCanvasDrawAndSave. This version fits a Gauss
	 */
	TCanvas * createCanvasFitDrawAndSave(TH1 * h, TString filename, TString basename, bool save = true, int fit = 1) {
		return createCanvasDrawAndSave(h, filename, basename, save, 1);
	}
	/**
	 * @brief See andi::createCanvasDrawAndSave. This version fits a double Gauss.
	 */
	TCanvas * createCanvasFitDoubleDrawAndSave(TH1 * h, TString filename, TString basename, bool save = true, int fit = 2) {
		return createCanvasDrawAndSave(h, filename, basename, save, 2);
	}
	/**
	 * @brief See andi::createCanvasDrawAndSave. For TH2, moved the Z axis to the left as well.
	 */
	TCanvas * createCanvasDrawAndSave(TH2 * h, TString filename, TString basename, bool save = true) {
		TString canvasName = h->GetName();
		canvasName.Remove(0, 1);
		canvasName.Prepend("c");
		TCanvas * c = new TCanvas(canvasName, filename, 0, 0, 800, 500);
		h->Draw("COLz");
		andi::makePadTitleAndDraw(h, (TPad*) c->GetPad(0));
		if (save) andi::saveCanvas(c, basename);
		return c;
	}
	/**
	 * @brief See andi::createCanvasDrawAndSave. For everything which comes from TObject. Like TGraph and sorts.
	 */
	TCanvas * createCanvasDrawAndSave(TObject * h, TString filename, TString basename, bool save = true, TString drawParams = "") {
		TString canvasName = h->GetName();
		canvasName.Remove(0, 1);
		canvasName.Prepend("c");
		TCanvas * c = new TCanvas(canvasName, filename, 0, 0, 800, 500);
		h->Draw(drawParams);
		andi::makePadTitleAndDraw(h);
		if (save) andi::saveCanvas(c, basename);
		return c;
	}
	/**
	 * @brief See andi::createCanvasDrawAndSave. For THStack. Also creates a legend.
	 */
	TCanvas * createCanvasDrawAndSave(THStack * stack, TString filename, TString basename, bool save = true, TString stackTitle = "Histograms", double boxStartY = 0.7, double boxStartX = 0.7) {
		TString canvasName = stack->GetName();
		canvasName.Remove(0, 1);
		canvasName.Prepend("c");
		TCanvas * c = new TCanvas(canvasName, filename, 0, 0, 800, 500);
		// stack->SetStats(kFALSE);
		stack->GetHistogram()->SetStats(false);
		stack->Draw("NOSTACK");
		andi::makePadTitleAndDraw((TH1*)stack);
		TLegend * leg = andi::stackLegend(stack, stackTitle, boxStartY, boxStartX);
		leg->Draw();
		if (save) andi::saveCanvas(c, basename);
		return c;
	}
	/**
	 * @brief Reads in a Tree from multiple Files
	 * @details For data collections of the same structure it is possible to append the trees after one another. This functions gives a convenient way to do so. The fileName parameter should point to a text file with the file name of a root file per line. The function returns a TTree with combined content.
	 * 
	 * @param treeName Name of the tree in every file. Has to be equal.
	 * @param fileName A text file with one file per line.
	 * 
	 * @return TTree of a chain of input files. The intermediate TChain is casted to a TTree, because of my macros.
	 */
	TTree * treeFromMultipleFiles(TString treeName, TString fileName) {
		TChain * tempChain = new TChain(treeName);
		TFileCollection collection("somename", "", fileName);
		tempChain->AddFileInfoList(collection.GetList());
		return (TTree*) tempChain;
	}

	/**
	 * @brief My cut benchmarks.
	 */
	namespace cuts {

		double sigBkgRatio(double sig, double bkg) {
			return sig * sig / (bkg + sig);
		}
		/**
		 * @brief Cut benchmark. For list of limits
		 * @details Finds the best signal to background ratio for a quantity and several limits, returns a graph of the ratios.
		 * Internal verbose parameter needs to be switched on manually.
		 * 
		 * There are different version of this method, for different uses cases. Usually only the steps parameter is different, e.g. sometimes a list of values, sometimes a range. The working principle of all methods is the same, though.
		 * 
		 * @param treeSig TTree holding testCut quantity to benchmark for SIGNAL
		 * @param treeBkg TTree holding testCut quantity to benchmark for BACKGROUND
		 * @param scaleFactorBkg Value with which the background is scaled
		 * @param testCut The cut which is tested, e.g. 'mass'
		 * @param defaultCut A pre-cut applied to all measurements of entries. Choose an empty string, if you don't need to apply any cuts. I'd like to define a TCut cutNone = ""
		 * @param steps Vector of quantities, e.g. {1, 2, 3}
		 * @param bestRatio The best ratio will be filled into this variable
		 * @return Graph with the S/B ratio for every evaluated point
		 */
		TGraph * cutBenchmark(TTree * treeSig, TTree * treeBkg, double scaleFactorBkg, TCut testCut, TCut defaultCut, std::vector<double> steps, double & bestRatio) {
			TGraph * tempGraph = new TGraph();
			int iPoint = 0;
			double lastRatio = -1.;
			bool verbose = false;
			for (std::vector<double>::iterator it = steps.begin(); it != steps.end(); it++, iPoint++)	{
				TCut currentCut = TString::Format("%s > %f", (char*) testCut, *it);
				if (verbose) std::cout << (char*)currentCut << std::endl;
				double entriesSig = treeSig->GetEntries(currentCut && defaultCut);
				if (verbose) std::cout << entriesSig << std::endl;
				double entriesBkg = scaleFactorBkg * treeBkg->GetEntries(currentCut && defaultCut);
				if (verbose) std::cout << entriesBkg << std::endl;
				double ratio = sigBkgRatio(entriesSig, entriesBkg);
				if (verbose) std::cout << ratio << std::endl;
				if (ratio > lastRatio) {
					bestRatio = *it;
					lastRatio = ratio;
				}
				tempGraph->SetPoint(iPoint, *it, ratio);
			}
			return tempGraph;
		}
		/**
		 * @brief Cut benchmark. For a simple upper limit and lower limit. Deprecated, as this can also be done with the other functions.
		 **/
		TGraph * cutBenchmark(TTree * treeSig, TTree * treeBkg, double scaleFactorBkg, TCut testCut, TCut defaultCut, std::pair<double, double> rangeLimits, Int_t numberOfSteps) {
			TGraph * tempGraph = new TGraph();
			double stepWidth = (rangeLimits.second - rangeLimits.first) / (double)numberOfSteps;
			int iPoint = 0;
			bool verbose = false;
			for (double i = rangeLimits.first; i < rangeLimits.second; i += stepWidth, iPoint++)	{
				if (verbose) std::cout << i << std::endl;
				TCut currentString = TString::Format("%s > %f", (char*) testCut, i);
				TCut currentCut = currentString;
				if (verbose) std::cout << (char*)currentCut << std::endl;
				double entriesSig = treeSig->GetEntries(currentCut && defaultCut);
				if (verbose) std::cout << entriesSig << std::endl;
				double entriesBkg = scaleFactorBkg * treeBkg->GetEntries(currentCut && defaultCut);
				if (verbose) std::cout << entriesBkg << std::endl;
				double ratio = sigBkgRatio(entriesSig, entriesBkg);
				if (verbose) std::cout << ratio << std::endl;
				tempGraph->SetPoint(iPoint, i, ratio);
			}
			return tempGraph;
		}
		/**
		 * @brief Cut benchmark. For a symmetrical range around a central value.
		 **/
		TGraph * cutBenchmarkSymmetric(TTree * treeSig, TTree * treeBkg, double scaleFactorBkg, TCut testCut, TCut defaultCut, double centralValue, double rangeLimit, Int_t numberOfSteps, double & _lowerLimit, double & _upperLimit) {
			TGraph * tempGraph = new TGraph();
			double stepWidth = rangeLimit / (double)numberOfSteps;
			double lastRatio = -1.;
			bool verbose = false;
			for (int i = 1; i <= numberOfSteps; i++)	{
				if ((i % 100) == 0) std::cout << "Loop: " << i << std::endl;
				double upperLimit = centralValue + i * stepWidth;
				double lowerLimit = centralValue - i * stepWidth;
				TCut currentCut = TString::Format("%s > %f && %s < %f", (char*) testCut, lowerLimit, (char*) testCut, upperLimit);

				if (verbose) std::cout << "Cut: " << (char*)currentCut << std::endl;
				double entriesSig = treeSig->GetEntries(currentCut && defaultCut);
				if (verbose) std::cout << "#Sig: " << entriesSig << std::endl;
				double entriesBkg = scaleFactorBkg * treeBkg->GetEntries(currentCut && defaultCut);
				if (verbose) std::cout << "#Bkg: " << entriesBkg << std::endl;
				double ratio = sigBkgRatio(entriesSig, entriesBkg);
				if (verbose) std::cout << "Ratio: " << ratio << std::endl;
				if (verbose) std::cout << "upperLimit: " << upperLimit << std::endl;
				if (entriesSig == 0) ratio = 0;
				if (entriesBkg == 0) ratio = 0;
				if (ratio > lastRatio) {
					_lowerLimit = lowerLimit, _upperLimit = upperLimit;
					lastRatio = ratio;
				}
				tempGraph->SetPoint(i - 1, upperLimit - lowerLimit, ratio);
				
			}
			return tempGraph;
		}
	}

	// Following functions are for my specific analysis
	/**
	 * @brief The properties of a particle
	 * @details Small struct with publicly accessible member variables of the most important quantities of a particle.
	 * 
	 */
	struct properties {
		Float_t pt, px, py, pz, p, E;
		Float_t m, chg, pdg;
	};
	/**
	 * @brief A container for my whole D meson decay
	 * @details Every decay arm has four particles: The actual D meson (the mother particle, m, and three daughters, d{0..2} (kaon, pion, pion)). All have the usual particle properties, as defined by the corresponding struct
	 * 
	 */
	struct DInfoContainer {
		properties m; // actual d meson (m=mother)

		properties d0, d1, d2; // three daughters
	};
	/**
	 * @brief Sets my needed branch addresses when running over NTuple explicitly
	 * @details My experiment has data files containing NTuple / TTrees with lots of branches corresponding to different properties of different particles. This function maps a info container to the corresponding branches.
	 * 
	 * @param tuple Pointer to the TTree holding all the info (ALL the info)
	 * @param container Address of a info container with all the needed methods
	 * @param baseString The string prefixing all the branches
	 * @param alsoDaugthers Set also daughter branches
	 */
	void setBranchAddresses(TTree * tuple, andi::DInfoContainer & container, TString baseString, bool alsoDaugthers = true) {
		tuple->SetBranchAddress(baseString + "pt", &(container.m.pt));
		tuple->SetBranchAddress(baseString + "px", &(container.m.px));
		tuple->SetBranchAddress(baseString + "py", &(container.m.py));
		tuple->SetBranchAddress(baseString + "pz", &(container.m.pz));
		tuple->SetBranchAddress(baseString + "p", &(container.m.p));
		tuple->SetBranchAddress(baseString + "e", &(container.m.E));
		tuple->SetBranchAddress(baseString + "m", &(container.m.m));
		tuple->SetBranchAddress(baseString + "pdg", &(container.m.pdg));
		tuple->SetBranchAddress(baseString + "chg", &(container.m.chg));

		if (alsoDaugthers) {
			tuple->SetBranchAddress(baseString + "d0" + "pt", &(container.d0.pt));
			tuple->SetBranchAddress(baseString + "d0" + "px", &(container.d0.px));
			tuple->SetBranchAddress(baseString + "d0" + "py", &(container.d0.py));
			tuple->SetBranchAddress(baseString + "d0" + "pz", &(container.d0.pz));
			tuple->SetBranchAddress(baseString + "d0" + "p", &(container.d0.p));
			tuple->SetBranchAddress(baseString + "d0" + "e", &(container.d0.E));
			tuple->SetBranchAddress(baseString + "d0" + "m", &(container.d0.m));
			tuple->SetBranchAddress(baseString + "d0" + "pdg", &(container.d0.pdg));
			tuple->SetBranchAddress(baseString + "d0" + "chg", &(container.d0.chg));

			tuple->SetBranchAddress(baseString + "d1" + "pt", &(container.d1.pt));
			tuple->SetBranchAddress(baseString + "d1" + "px", &(container.d1.px));
			tuple->SetBranchAddress(baseString + "d1" + "py", &(container.d1.py));
			tuple->SetBranchAddress(baseString + "d1" + "pz", &(container.d1.pz));
			tuple->SetBranchAddress(baseString + "d1" + "p", &(container.d1.p));
			tuple->SetBranchAddress(baseString + "d1" + "e", &(container.d1.E));
			tuple->SetBranchAddress(baseString + "d1" + "m", &(container.d1.m));
			tuple->SetBranchAddress(baseString + "d1" + "pdg", &(container.d1.pdg));
			tuple->SetBranchAddress(baseString + "d1" + "chg", &(container.d1.chg));

			tuple->SetBranchAddress(baseString + "d2" + "pt", &(container.d2.pt));
			tuple->SetBranchAddress(baseString + "d2" + "px", &(container.d2.px));
			tuple->SetBranchAddress(baseString + "d2" + "py", &(container.d2.py));
			tuple->SetBranchAddress(baseString + "d2" + "pz", &(container.d2.pz));
			tuple->SetBranchAddress(baseString + "d2" + "p", &(container.d2.p));
			tuple->SetBranchAddress(baseString + "d2" + "e", &(container.d2.E));
			tuple->SetBranchAddress(baseString + "d2" + "m", &(container.d2.m));
			tuple->SetBranchAddress(baseString + "d2" + "pdg", &(container.d2.pdg));
			tuple->SetBranchAddress(baseString + "d2" + "chg", &(container.d2.chg));
		}		
	}

}
