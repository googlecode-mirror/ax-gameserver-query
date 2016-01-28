  1. Goto your php source folder and open the "ext" folder.
  1. Create a new folder called "ax\_gameserver\_query".
  1. Copy all the files from the projects trunk into the folder.
  1. Go back to the php source folder.
  1. Run "./buildconf --force".
  1. Once thats finished run "./configure --enable-ax\_gameserver\_query" (remeber to add any aditional switches for other extensions that you require).
  1. Now run the "make" command followed by the "make install" command to compile then install php.
  1. Now you done.