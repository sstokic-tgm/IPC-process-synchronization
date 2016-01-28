# IPC


IPC process synchronization example, using 2 different methods.

Methods:
- Named Conditions
- message_que


The project is using the 1.59 boost library. Please link boost proper to your project!


###### Steps under Windows Visual Studio:
- In the project properties under "VC++ Directories", add the Include-Directory YOUR_BOOST_INSTALLATION_DEST\include\boost-1_59
- In the project properties under "VC++ Directories", add the Library-Directory YOUR_BOOST_INSTALLATION_DEST\lib
- In the project properties under "C/C++/General", add the Additional Include Directory YOUR_BOOST_INSTALLATION_DEST\include\boost-1_59\boost
- In the project properties "Linker/General", add the Additional Library Directory YOUR_BOOST_INSTALLATIO_DEST\lib
- In the project properties "C/C++/Preprocessor", add the following definitions:
  - _CRT_SECURE_NO_WARNINGS
  - BOOST_ALL_DYN_LINK

###### Steps under unix(using g++):
- First you need to install boost with the Package Manager (under Arch Linux ```sudo pacman -S boost boost-libs```
- Then compile (depended on the unix system you need to add the -pthread flag):
```g++ <cpp files> -o <output file> -std=c++11 -lboost_system -lrt [-pthread]```
