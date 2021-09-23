# NAME - texteditor

# SYNOPSIS -
man texteditor [filename]

# DESCRIPTION -
This is a text editor that displays the current line given a file, and thereby updates based on commands the user can give.

# OPTIONS -
        p: prints contents of the file to standard output
        n: prefixes each line of the file with a line number while also printing to standard output
        d: deletes the current line and can apply to a range of line numbers
        m: moves the current line to the line before given int, but causes an error if exceeding the int given
        i: inserts line before current line
        a: inserts line after current line
        w: saves any change(s) made
        q: quits the editor without saving
	s: searches for a substring, starting from the line after the current line and progresses through to the end of the file - if the particular string is located, the current 	      line is now the new line on which the string is located
	|: a vertical bar command that allows a sub-process to be executed within a file buffer as the standard input initially, resulting in standard output - For example, the 
	   |tr command followed by lower to upper-case test would complete that subb-process within the buffer or something such as a sed command could be executed for the purpose	      of replacing a string of characters   


# VERSION - 
This is the third version of a texteditor.c file in which additional commands are added, demonstrating more functionalities which allow for searching (use case mentioned above) aswell as using the vertical bar option. 


BY  Akriti Keswani
