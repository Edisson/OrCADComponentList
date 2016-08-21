# OrCADComponentList
Create list of electronic components for the further drawing up of documents in State Standard Documentation by template.

If you are working with BOM-files of OrCAD creating specification this utility might be very helpful to save your time.

***
This project is developed on Qt platform.
Please use Qt version 5.x or higher

Description:

Any IDE for developing PCB design (printed circuit board) and electronic schemes can generate an output file with all the components in its project. Engineers use content from this file to prepare technical documents and specifications.
Basically we can't use this "raw" data from file creating State Standard Documentation for PCB and have to reissue this data.

For instance OrCAD can generate component list in this kind of format [columns]:

[ordinal number] [counter of components] [list of components (names)] [nominal] [case]

"case" here is a dimension type or form factor.

In the folder "orcad_demo_files" you can find the file "MPV7266_3.BOM" with components from a project. It's just simple text file with another extension.

For example in the second row of data you can see 61 capacitors (C1.. exactly how they are enumerated on an electronic scheme). Further you see a nominal value - 0.1mk and case of the component - SMD_0805. 

In the next lines there are another capacitors C25 - C28 and have different nominal value, that's why they are not in the previous list.

What engineer want to see in the specification is a table with components in another format (with spaces and description)
[mixed component list] [case] [nominal and description] [counter]

for example in the output file we will find prepared lines:
[C1...C24]	[SMD 0805] [0.1 мкФ 5%]	[24]
[C29]	[SMD 0805] [1 мкФ 5%]	[1]
[C30]

and so on...

Developer have to make a reference file where the application can get some extra info for "regular expressions" to parse lists of  components and description correctly.
In the demo folder you fill find file "ref.bom" that is a reference file.

For example 
C - means capacitors 
*mk - means that on scheme there are capacitors with label [number]mK and also case SMD_0805

if the application find suitable component it get the number (nominal) from *mk string and replace asterix in the last column [*_мкФ_5%] of the reference file.

Another important and interesting option of the application is that it creates groups of components such as [C1...C24] if components are following in consecutive order and there are more than 2 components in the list, otherwise they separate by comma.
Another reason to interrupt sequence is different nominal value or case for the particular component.

Unfortunately other CAD systems generate another lists and utility require improvement to process them.

Demo files include cyrillic because it was made for russian electronicians;)