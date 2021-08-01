scriptname f314PF_DemoScript extends activemagiceffect

import f314PF_PrintF

string   property format_prop  auto
string[] property args_prop    auto
string   property russian_prop auto

function printf(string s, bool msg=false)
  if msg
    debug.MessageBox(s)
  else
    debug.notification(s)
  endif
  debug.trace(s)
endfunction

string function genHelloPart(string comma="", string end="!")
  return "Hello" + comma + " %s" + end
endfunction

string function genAvPart(string av="hp", string type="%.1f")
  return "Your " + av + " is " + type + "."
endfunction

function arrayDemo()
  Actor Player = game.getplayer()
  
  ; gen stuff on fly
  string[] helloargs = new string[5]
  helloargs[0] = Player.GetBaseObject().GetName()
  helloargs[1] = Player.GetActorValue("Health") as string
  helloargs[2] = Player.GetLevel() as string
  helloargs[3] = "3"  ; precision of PI to show
  float pi = 31415.9265358
  helloargs[4] = pi as string
  string generatedformat = genHelloPart(",") + " " + genAvPart() + " " + genAvPart("Level", "%d")
  printf(PrintFArr(generatedformat + " btw, PI=%.*f", helloargs))
  
  ; just hardcoded (not array)
  
  ; read from property
  printf(PrintFArr(format_prop, args_prop))
endfunction

function stringDemo()
  int a = 2
  int b = 3
  int c = a + b
  printf(PrintFStr("If you want just hardcoded output: %d %s %d = %d", a + "`+`" + b + "`" + c))
  printf(PrintFStr("Or even '%d %c %d = %d'", "2`+`2`4"))
  
  ; easy translation
  string arrname = "Myarray"
  string left = 10 as string
  string right = 31 as string
  string result = "hello"
  printf(PrintFStr(russian_prop, "translate`" + arrname + "`" + left + "`" + right + "`" + result))
endfunction

function msgboxDemo()
  Actor Player = game.getplayer()
  
  string[] avs  = new string[8]
  int size = avs.length
  avs[0] = "Health"
  avs[1] = "Magicka"
  avs[2] = "Stamina"
  avs[3] = "Restoration"
  avs[4] = "Illusion"
  avs[5] = "Alteration"
  avs[6] = "Destruction"
  avs[7] = "Conjuration"
  
  string[] args = new string[16]
  string format = "Now you can easy create this:"
  
  int i = 0
  while i < size
    format = format + "\n" + "%-15s:" + "%4.1f"
    args[2 * i]     = avs[i]
    args[2 * i + 1] = Player.GetActorValue(avs[i])
    i += 1
  endwhile
  
  printf(PrintFArr(format + "\n(yes I hate non-monospaced fonts)", args), true)
endfunction

function variadicDemo()
  printf(PrintF0("But the simplest way isusing variadic functions"))
  printf(PrintF9("They called %s %s %s %s %s %s %s %s %s and PrintF0", "PrintF1", "PrintF2", "PrintF3", "PrintF4", "PrintF5", "PrintF6", "PrintF7", "PrintF8", "PrintF9"))
endfunction

event oneffectstart(Actor akTarget, Actor akCaster)
  arrayDemo()
  stringDemo()
  msgboxDemo()
  variadicDemo()
endevent
