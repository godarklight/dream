
About Translation:

Translation, also called localization (or l10n for short), is a task that can be contributed by anyone. No programming skills are required. Any language that is not English (original messages) can be contributed.

Localization software helps translate the strings that appear inside the application, including menus, buttons, and dialog boxes. All of the user visible strings are extracted from the Dream source code and placed into a "source message file" file called dreamtr.ts. The localization software allows you to translate the strings in this file.


Translation instructions:

Before starting a new translation, ask first on the Sourceforge Dream forum if somebody else is doing it for that language. If no, then proceed.

First, you will need to download latest Dream CVS or upcoming version 1.2.

In the following sections, the XX code refers to the code for your language (da=Danish, de=German, en=English, es=Spanish, fi=Finnish, fr=French, it=Italian, nl=Dutch, no=Norwegian, ru=Russian, sv=Swedish, ..). 

Under Linux/Uni*x:
All the TS files are under common/GUI-QT/languages/dreamtr.XX.ts (in CVS repository). In linux/ subdir, run "make languages/dreamtr.XX.ts" to either create or update/merge the TS file. Edit the linux/languages/dreamtr.XX.ts file and to test it, run "make languages/dreamtr.XX.qm" to create the QM release file. Copy that QM file as dreamtr.qm and restart Dream program to use it. When your are satisfied with the translation, then the TS file has to be copied back into common/GUI-QT/languages and use "cvs diff -u" or alike for patch submitting.


Under Windows:
To start a new translation, download the latest file of the language drm/common/GUI-QT/languages/dreamtr.XX.ts. If this is a translation for a new language, download the file drm/linux/dreamtr.ts instead. Rename it dreamtr.XX.ts when you begin translating, where XX is the code explained above. 

When you have begun translating the source message file (a TS file), use your translation program to compile a light-weight binary message file (a QM file) from the TS file, suitable only for end use. This will produce a file called dreamtr.XX.qm. 


Providing translations (Linux/Windows):
Provide translations for the source texts in the TS file. 

The source message file (TS file) is a XML format file. Even though it can be edited by regular text editor, it is recommended to use a specialized tool for ease of use and better support of Unicode charset.

There's a nice Qt GUI named 'linguist'[1] to edit translations. Debian users may simply "apt-get install qt3-linguist". Linguist comes with Qt Phrase Books (QTH files) with translations from common words of many languages. According to Trolltech site[2], it works for Qt2/Windows. I'm curious also if it would handle japanese translation or any non-latin based languages. Anyone from the country of the rising sun?
KBabel[3], has support for Qt files too.
To create the QM file from the TS file, you can also use Qt-provided 'lrelease' cmdline tool.

[1] http://www.trolltech.com/company/announce/linguistpre.html
[2] http://doc.trolltech.com/2.3.9/i18n.html
[3] http://i18n.kde.org/tools/kbabel/

Notes:
* Strings like "&File" and "New &Audio Track" are menu items. The letter after the & character is used as a keyboard shortcut on Windows and Unix.

* The HTML tags "<b></b>" etc. have to be reproduced at the appropriate places in the translation.

* Be careful as some texts may have space constraints in the Graphical User Interface.

* You will find some strings like "%1 of %2 files copied.\nCopying: %3". The % signs mark places where additional information will be inserted into the string. You are allowed to change the order of the % markers in the translation if the programmer did his/her work right.



User Point of view:

Download the QM file for the language of your choice. You may also re-generate it from a TS file.
In the same folder as the Dream executable, rename your dreamtr.XX.qm file to dreamtr.qm. Restart the Dream program. If dreamtr.qm exists it is loaded, otherwise the internal text (English) is used.



Programmer point of view:

To the programmer's point of view, to get translation working, just replace all "" literals by tr(""). tr() is based on QString with internal Unicode support. Unlike GNU gettext, the translating library is already included in Qt. A QTranslator is initialized at startup of the program. Then binary translation files (QM file) have to be distributed along for translation to take place. That's all. 
 
The translated text is "plugged" (at runtime) into the code by the tr() function (which tr name is short to not clutter the code). If no translation is available for the argument, the original text is returned, otherwise it is retrieved from a fast access catalog. 

Run "make -C linux/ dreamtr.ts" to extract translatable text from the C++ source code of the Qt application, resulting in a source message file (a TS file).

A Qt tool "lupdate" automatically scans the .cpp/.h/.ui files and extract the string literals within the tr() argument.


---
This text was written by Stephane Fillod

