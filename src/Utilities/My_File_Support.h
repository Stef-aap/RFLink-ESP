

#ifndef My_File_Support_h
#define My_File_Support_h

// **************************************************
// Parse the filename and store the parts
// **************************************************
int Parse_FileNr(String Filename) {
  int File_Nr = 0;
  if (Filename.length() == 0) return File_Nr;

  int x1 = Filename.indexOf('_');
  int x2 = Filename.indexOf('.');
  if ((x1 > 0) && (x2 > x1)) {
    String Part = Filename.substring(x1 + 1, x2);
    File_Nr = Part.toInt();
  }
  return File_Nr;
};

#endif
