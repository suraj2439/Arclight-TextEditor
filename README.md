Project: Text Editor For ‘C’ Language


Overview:
Use like normal Text Editor, for ‘C’ programmer it will be little special.
Can open bigger size text files which cannot be opened by vim, gedit, nano, kate like text editors.
GUI using Ncurses library. 

For ‘C’ programmers:
‘C’ language syntax highlighting.
Many shortcut keys for commonly used syntax.
Codebase facility to store code in codebase permanently using key and easily access it later .


Data Structures Used:
Circular Array
Linked List of Array
Gap Buffer
Trie
Stack

Internal Structure:
Internal Structure consist of, Array of structures having pointers pointing to particular  line structure.
Line structure is implemented using linked list of Array to store infinite size line, 
to decrease cost of common operations like insertion , deletion internally each Array work with Gap Buffers.
To implement multiple window, outer window structure is available.
User is allowed to work only in this Data structure.
Now to reduce ram usage, Data structure corresponding to only one window will be stored in ram, 
to handle other file data two temporary files are used. 


Normal Text Editor Features:
Editor does not allow to write in ‘Read Only’ file, only navigation is allowed in such files.

Check if input file is already present or not, if present load that file in data structure 
otherwise create new file with input name in ‘Read Write mode’.

Insertion: To insert data in ADT, move gap buffer cursor at given point, 
data will be inserted at left boundary of gap buffer, increase left boundary of gap buffer which will decrease gap size.
If gap size is already zero, move gap buffer cursor at given point, 
insert new node in between current array and next array which will have gap size equal to array size, 
copy right side data of current array to increase current array gap size(previously zero), 
now insert data in current array as mention in first point.


Insertion continued...
Time Complexity: 
Best Case: O(n)
Worst Case: O(n+2p) 
where n = number of nodes in linked list of array,
p = size of one array of linked list of array.

Deletion: To delete data from ADT, move gap buffer cursor at given point, 
decrease left boundary of gap and data will go inside gap which will increase gap size.
 Gap buffer data will not be shown in screen.
Time Complexity: 
Best Case: O(n)
Worst Case: O(n+p) where n = number of nodes in linked list of array,
p = size of one array of linked list of array.(constant)


Load Previous Line: If user press up arrow at top, Last line of ADT will be stored in to 
filename_next.tmp file to make space for one line. Now previous line will be fetched from filename_prev.tmp file.
Best Case & Worst Case: O(n+p) - constant
where n = number of lines in window(constant) 

Load Next Line: If user press down arrow at bottom, First line of ADT will be stored into 
filename_prev.tmp file to make space for next line in ADT, now if present next line will be fetched from 
filename_nxt.tmp file, otherwise it will be fetched from main file.
Best Case & Worst Case: O(n+p) - constant
where n = number of lines in window(constant)
p = combine time to write and read one line from file 


Undo: For undo operation stack is used, same opertions performed successively will be undo at one time.
At each node of stack type of operation, its frequency and last position is stored.
Contents will be stored in stack after each operation performed.
Time Complexity: Depends upon type of operation which is to be undo

Save: To save file new file is created, first tmp_prev file data, then data structure data ,
then tmp_next file data and at last remaining main file data will be written in newly created file. 
All permission will be copied from main file and applied to newly created file then main file and all 
temparory files are removed and new file is renamed as main file.
Time Complexity: O(n)
where n = number of characters in updated file.


Quit Without Save: All the temporary files will be removed and program will quit without touching main file.
Time Complexity: O(1)

Some Shortcuts:
Page Up: Scroll to next page(window)
Page Down: Scroll to previous page(window)
Ctrl + H = move to start of current line
Ctrl + L= move to end of current line
Ctrl + O = move to top line of window
Ctrl + K = move to bottom line of window


Brackets are inserted in pair.

Syntax highlighting: All ‘C’ language keywords are highlighted with group of color, 
similar keywords have given same color.
 It is implemented using Trie data structure, each keyword with its color is stored in trie, 
 at time of printing data on screen word is first searched in this keyword trie if matched 
 that word is printed with appropriate color otherwise printed with white color.
Time Complexity: O(n)
where n = length of key given to search.


Some features for ‘C’ programmer:

Shortcut keys            Insert
Ctrl + P                printf("%",);
Ctrl + A                scanf("%", &);
Ctrl + E                #include <>;
Ctrl + D                #define 
Ctrl + N                main loop
Ctrl + F                for(int i = ; )
Ctrl + W                while()
Ctrl + R                return ;
Ctrl + T                typedef struct {\n \n} ;
Ctrl + ;                insert ‘;’ semicolon at end of line
Cursor will be adjusted at appropriate position.
Time Complexity: O(1)


CodeBase Feature: Some common functions are always required during ‘C’ programming, 
we write codes but there is no simple solution to store such codes and access easily whenever required.
Such functions or code can be added in CodeBase file with input key given by user, 
this key will be stored in .codebase.key file with start and end offset of stored data, 
data will be stored in .codebase.data file.
At start of editor all keys are loaded in Trie with lower and upper offset of its data 
which is stored in .codebase.data file, when user enters this key, it is searched in Trie if matched 
corresponding data using lower and upper offset will be inserted at current position.


Codes will be stored permanently in codebase if we store some piece of code at start in codebase with some key, 
now we can access this code at any time in any file by providing that key.
This feature will become very powerful when codebase gradually increases, after some time we will 
never require to write common functions or codes.
This facility can be used for any programming language as well as for normal text file to store commonly required text.
Time Complexity: O(n)
where n = length of key given to search.


