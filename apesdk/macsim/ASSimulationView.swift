/****************************************************************
 
 ASSimulationView.swift
 
 =============================================================
 
 Copyright 1996-2021 Tom Barbalet. All rights reserved.
 
 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated documentation
 files (the "Software"), to deal in the Software without
 restriction, including without limitation the rights to use,
 copy, modify, merge, publish, distribute, sublicense, and/or
 sell copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following
 conditions:
 
 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.
 
 This software is a continuing work of Tom Barbalet, begun on
 13 June 1996. No apes or cats were harmed in the writing of
 this software.
 
 ****************************************************************/

import Cocoa

class ASSimulationView: ASMacView {

    func uniformOpenPanel() -> NSOpenPanel {
        let panel = NSOpenPanel()
        panel.allowedFileTypes = ["txt"]
        panel.canChooseDirectories = false
        panel.allowsMultipleSelection = false
        
        NSLog("Obtaining and returning uniform open panel")
        
        return panel
    }

    func uniformSavePanel() -> NSSavePanel {
        let panel = NSSavePanel()
        panel.allowedFileTypes = ["txt"]
        
        NSLog("Obtaining and returning uniform save panel")
        
        return panel
    }

    override func awakeFromNib() {
        super.awakeFromNib()
        super.startView()
        shared?.identificationBasedOnName(windowName: window?.title ?? "")
        shared?.startEveything(headyLifting: shared?.identification ?? 0 == WINDOW_PROCESSING, window: self.localWindow)
    }

    func renderTime(inOutputTime: CVTimeStamp) -> CVReturn {
        DispatchQueue.main.async { [weak self] in
            self?.needsDisplay = true
        }
        return kCVReturnSuccess
    }

    func loadUrlString(_ urlString: String) {
        NSWorkspace.shared.open(URL(string: urlString)!)
    }

    func debugOutput() {
        DispatchQueue.main.async { [weak self] in
            guard let self = self else { return }
            let panel = self.uniformSavePanel()
            NSLog("Obtaining debug output")
            panel.begin { result in
                if result == .OK {
                    self.shared?.scriptDebugHandle(panel.url?.path ?? "")
                }
            }
        }
    }

    // MARK: - IB Actions

    func menuCheckMark(_ sender: AnyObject?, check value: Int) {
//        if sender?.responds(to: #selector(setter: NSControl.state)) == true {
//            (sender as? NSControl)?.state = value == 1 ? .on : .off
//        }
    }

    @IBAction func menuFileNew(_ sender: Any?) {
        shared?.newSimulation()
        NSLog("Finished new landscape")
    }

    @IBAction func menuFileNewAgents(_ sender: Any?) {
        shared?.newAgents()
        NSLog("Finished new agents")
    }

    @IBAction func menuFileOpen(_ sender: Any?) {
        let panel = uniformOpenPanel()
        panel.begin { result in
            if result == .OK {
                if self.shared?.openFileName(panel.url?.path ?? "", isScript: false) == false {
                    NSSound(named: "Pop")?.play()
                }
            }
        }
    }

    @IBAction func menuFileOpenScript(_ sender: Any?) {
        let panel = uniformOpenPanel()
        panel.begin { result in
            if result == .OK {
                if self.shared?.openFileName(panel.url?.path ?? "", isScript: true) == false {
                    NSSound(named: "Pop")?.play()
                }
            }
        }
    }

    @IBAction func menuFileSaveAs(_ sender: Any?) {
        let panel = uniformSavePanel()
        panel.begin { result in
            if result == .OK {
                self.shared?.savedFileName(panel.url?.path ?? "")
            }
        }
    }

    @IBAction func menuControlPause(_ sender: Any?) {
        menuCheckMark(sender as AnyObject, check: shared?.menuPause() ?? 0)
    }

    @IBAction func menuControlFollow(_ sender: Any?) {
        menuCheckMark(sender as AnyObject, check: shared?.menuFollow() ?? 0)
    }

    @IBAction func menuControlSocialWeb(_ sender: Any?) {
        menuCheckMark(sender as AnyObject, check: shared?.menuSocialWeb() ?? 0)
    }

    @IBAction func menuControlPrevious(_ sender: Any?) {
        shared?.menuPreviousApe()
    }

    @IBAction func menuControlNext(_ sender: Any?) {
        shared?.menuNextApe()
    }

    @IBAction func menuControlClearErrors(_ sender: Any?) {
        shared?.menuClearErrors()
    }

    @IBAction func menuControlNoTerritory(_ sender: Any?) {
        menuCheckMark(sender as AnyObject, check: shared?.menuNoTerritory() ?? 0)
    }

    @IBAction func menuControlNoWeather(_ sender: Any?) {
        menuCheckMark(sender as AnyObject, check: shared?.menuNoWeather() ?? 0)
    }

    @IBAction func menuControlNoBrain(_ sender: Any?) {
        menuCheckMark(sender as AnyObject, check: shared?.menuNoBrain() ?? 0)
    }

    @IBAction func menuControlNoBrainCode(_ sender: Any?) {
        menuCheckMark(sender as AnyObject, check: shared?.menuNoBrainCode() ?? 0)
    }

    @IBAction func menuControlDaylightTide(_ sender: Any?) {
        menuCheckMark(sender as AnyObject, check: shared?.menuDaylightTide() ?? 0)
    }

    @IBAction func menuControlFlood(_ sender: Any?) {
        shared?.menuFlood()
    }

    @IBAction func menuControlHealthyCarrier(_ sender: Any?) {
        shared?.menuHealthyCarrier()
    }

    @IBAction func menuCommandLine(_ sender: Any?) {
        shared?.menuCommandLineExecute()
    }

    @IBAction func loadManual(_ sender: Any?) {
        loadUrlString("http://www.nobleape.com/man/")
    }

    @IBAction func loadSimulationPage(_ sender: Any?) {
        loadUrlString("http://www.nobleape.com/sim/")
    }
}
