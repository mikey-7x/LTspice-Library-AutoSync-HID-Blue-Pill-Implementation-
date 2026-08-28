#include <Arduino.h>
#include <Keyboard.h>

void sendLine(const char* text) {
  Keyboard.print(text);
  delay(20);
  Keyboard.write(KEY_RETURN);
  delay(40);
}

void setup() {
  // 1. Hardware Fix: Force USB Re-enumeration
  pinMode(PA12, OUTPUT);
  digitalWrite(PA12, LOW);
  delay(300); 
  pinMode(PA12, INPUT); 
  delay(200);
  
  Keyboard.begin();
  delay(4000); 

  // 2. Open Run Dialog & Launch PowerShell
  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('r');
  delay(100);
  Keyboard.releaseAll();
  delay(1000); 

  sendLine("powershell");
  delay(3000); 

  // 3. Setup Directories & Download thyristr.lib
  sendLine("$d=\"$env:LOCALAPPDATA\\LTspice\\lib\"; $s=\"$d\\sub\"; $y=\"$d\\sym\\Thyristors\"");
  sendLine("md $s,$y -ea 0 | Out-Null");
  sendLine("iwr 'https://raw.githubusercontent.com/HMGrunthos/InrushTimer/master/thyristr.lib' -o \"$s\\thyristr.lib\"");

  // 4. LOAD NATIVE LTSPICE GRAPHICS FOR SCR & TRIAC
  sendLine("$t_scr = gc \"$d\\sym\\Misc\\SCR.asy\" -Raw");
  sendLine("$t_tri = gc \"$d\\sym\\Misc\\TRIAC.asy\" -Raw");

  // 5. HARDCODE PERFECT UJT GRAPHIC (N-Type Base, Base Overhang, Pins Outside Circle)
  sendLine("$t_ujt = \"Version 4`nSymbolType CELL`n\"");
  sendLine("$t_ujt += \"CIRCLE Normal -24 -24 24 24`n\"");
  sendLine("$t_ujt += \"LINE Normal -8 -20 -8 20`nLINE Normal -8 -16 24 -16`n\"");
  sendLine("$t_ujt += \"LINE Normal 24 -16 24 -32`nLINE Normal -8 16 24 16`n\"");
  sendLine("$t_ujt += \"LINE Normal 24 16 24 32`nLINE Normal -32 -8 -16 -8`n\"");
  sendLine("$t_ujt += \"LINE Normal -16 -8 -8 0`nLINE Normal -8 0 -14 0`n\"");
  sendLine("$t_ujt += \"LINE Normal -8 0 -8 -6`nWINDOW 0 -32 -24 Left 2`n\"");
  sendLine("$t_ujt += \"WINDOW 3 24 8 Left 2`nSYMATTR Prefix X`nSYMATTR Value {0}`n\"");
  sendLine("$t_ujt += \"SYMATTR ModelFile thyristr.lib`nPIN 24 -32 NONE 0`n\"");
  sendLine("$t_ujt += \"PINATTR PinName B2`nPINATTR SpiceOrder 1`n\"");
  sendLine("$t_ujt += \"PIN -32 -8 NONE 0`nPINATTR PinName E`nPINATTR SpiceOrder 2`n\"");
  sendLine("$t_ujt += \"PIN 24 32 NONE 0`nPINATTR PinName B1`nPINATTR SpiceOrder 3`n\"");

  // 6. APPLY YOUR STEP 5 & 6 (Fix Pin Routing and Symbol Attributes)
  
  // Fix SCR Pins (Swap G & K) & Attributes
  sendLine("$t_scr = $t_scr -replace 'PinName K\\s+PINATTR SpiceOrder \\d+', \"PinName K`nPINATTR SpiceOrder 3\"");
  sendLine("$t_scr = $t_scr -replace 'PinName G\\s+PINATTR SpiceOrder \\d+', \"PinName G`nPINATTR SpiceOrder 2\"");
  sendLine("$t_scr = $t_scr -replace 'SYMATTR Prefix \\w+', 'SYMATTR Prefix X'");
  sendLine("$t_scr = $t_scr -replace 'SYMATTR Value \\S+', 'SYMATTR Value {0}'");
  sendLine("if($t_scr -notmatch 'SYMATTR ModelFile') { $t_scr += \"SYMATTR ModelFile thyristr.lib`n\" } else { $t_scr = $t_scr -replace 'SYMATTR ModelFile \\S+', 'SYMATTR ModelFile thyristr.lib' }");

  // Fix TRIAC Pins (Swap G & MT1) & Attributes
  sendLine("$t_tri = $t_tri -replace 'PinName MT2\\s+PINATTR SpiceOrder \\d+', \"PinName MT2`nPINATTR SpiceOrder 1\"");
  sendLine("$t_tri = $t_tri -replace 'PinName G\\s+PINATTR SpiceOrder \\d+', \"PinName G`nPINATTR SpiceOrder 2\"");
  sendLine("$t_tri = $t_tri -replace 'PinName MT1\\s+PINATTR SpiceOrder \\d+', \"PinName MT1`nPINATTR SpiceOrder 3\"");
  sendLine("$t_tri = $t_tri -replace 'SYMATTR Prefix \\w+', 'SYMATTR Prefix X'");
  sendLine("$t_tri = $t_tri -replace 'SYMATTR Value \\S+', 'SYMATTR Value {0}'");
  sendLine("if($t_tri -notmatch 'SYMATTR ModelFile') { $t_tri += \"SYMATTR ModelFile thyristr.lib`n\" } else { $t_tri = $t_tri -replace 'SYMATTR ModelFile \\S+', 'SYMATTR ModelFile thyristr.lib' }");

  // 7. PARSE AND GENERATE THE ENTIRE LIBRARY
  sendLine("gc \"$s\\thyristr.lib\" | sls '(?i)^\\.subckt\\s+([\\w\\d/-]+)\\s+(.*)' | % {");
  sendLine("  $n = $_.Matches.Groups[1].Value");
  sendLine("  $p = $_.Matches.Groups[2].Value");
  sendLine("  $f = $n -replace '/','_'"); // Fixes illegal characters in filenames
  sendLine("  if ($p -match '(?i)MT2') { $out = $t_tri -f $n } elseif ($p -match '(?i)B2') { $out = $t_ujt -f $n } else { $out = $t_scr -f $n }");
  sendLine("  sc \"$y\\$f.asy\" $out");
  sendLine("}");
  sendLine("exit");

  Keyboard.end();
}

void loop() {}
