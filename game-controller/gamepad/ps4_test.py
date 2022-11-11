#game pad state is initialized as a "report"
#to press buttons, you modify the report and then call update()

#process is:
    #button press
    #update
    #sleep

    #button release
    #update
    #sleep

#one way of doing this could be just to read the state of every button on the microbit constantly
#and then just pass everything doing in the report
    

import vgamepad as vg

gamepad = vg.VDS4Gamepad()
