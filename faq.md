---
title: faq
---
# Frequently Asked Questions

**Q. My MAX7219 Displays are mirrored, rotated or just displaying crap, what can i do?**

A. Look in the MAX72XX Submodule folder and open the MAX72xx.h in edit mode and
    search for the MAX7219 module settings. There you can change the type of module you got
    and will hopefully fix the displaying issue.
    
**Q. The firmware want compile, it says "error: 'typedef' was ignored in this declaration"**

A. Apply the MD_Parola.patch from the repositorys root folder.
Or remove the "typedef" tag from MD_Parola.h by yourself

**Q. Can i see or download the configuration files for later use?**

A. You can see or download the configuration files in json format if you open
http://YOUR_ESP_IP/dumpcfg?type=TYPE 
and TYPE can be *mqtt*, *network* or *periph*