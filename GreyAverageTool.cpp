#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>

namespace fs = std::filesystem;

struct pgm_header {
    std::string magicn; //magic number of .pgm file
    std::size_t width=0; //width found in header of .pgm file
    std::size_t height=0; //height found in header of .pgm file 
    std::size_t maxval=0; //max. value (=white) defined in header of .pgm file
    bool wflag=0; //warning flag
};

pgm_header F_ExtractHeaderInfo(std::ifstream &pgmfile) 
{
    const std::string comment = "#"; //comment lines start with #
    const std::size_t elines = 3; //number of expected lines with at least one argument: Magic Number, width/height, Maxval
    std::size_t plines = 0; //so far processed lines with at least one argument for this .pgm file
    std::string line; //string iterating through the lines of the .pgm
    std::string first; //string for first character per line
    std::size_t found; //index for found space between width and height
    pgm_header newHeader; //struct for the header data

    if (pgmfile.is_open())  {
        std::getline(pgmfile, line); //get first line
        //search for the first three lines with each at least one argument
        while ((plines < elines) && (!line.empty()))    {
            first = line.substr(0,1);
            //Does first character indicate a comment (#)?
            if (first.compare(comment) != 0)    {
                plines += 1;
                switch (plines) {
                case 1: //magic number found
                    newHeader.magicn = line; 
                    break;
                case 2: //width, height found
                    found = line.find(" ");
                    if (found!=std::string::npos)   {
                        //space between widht and height found
                        newHeader.width = std::stoi(line.substr(0,found));
                        newHeader.height = std::stoi(line.substr(found+1));
                    }
                    else    {
                        newHeader.wflag = 1; //warning - second data line not according to .pgm definition
                    }
                    break;
                case 3: //maxval found
                    newHeader.maxval = std::stoi(line); 
                    break;
                default:
                    newHeader.wflag = 1; //warning
                    break;
                }
            }
            if(plines < elines) {
                std::getline(pgmfile, line); //get next line
            }
        }
    }
    else    {
        newHeader.wflag = 1; //warning
    }
    return newHeader;
}

int main(int argc, char* argv[])
{
    const std::string ext=".pgm"; //file extension .pgm
    const std::string exp_magicn = "P2"; //expected magic number .pgm file
    std::size_t exp_width = 1856; //expected width of .pgm file
    std::size_t exp_height = 1024; //expected height of .pgm file
    std::size_t exp_maxval = 4096; //expected maxval of .pgm file

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if ((arg == "-h") || (arg == "--help")) {
            std::cout << "This tool reads the path to a folder, reads all .pgm files in the folder and creates an average .pgm file in the same folder.\n\n"
                        << "Options:\n"
                        << "\t-h,--help  : Show this help message\n"
                        << "\t-w,--width : Specify the expected width of the .pgm file\n"
                        << "\t-l,--lines : Specify the expected lines/height of the .pgm file\n"
                        << "\t-m,--max   : Specify max. value of the .pgm file\n"
                        << std::endl;
            return 0;
        } else if ((arg == "-w") || (arg == "--width") || (arg == "-l") || (arg == "--lines")|| (arg == "-m") || (arg == "--max")) {
            if (i + 1 < argc) { // Making sure there is a next argument
                std::istringstream ss(argv[i+1]);
                std::size_t x;
                if (!(ss >> x) || !ss.eof()) {
                    std::cerr << "Invalid number or trailing character after number: " << arg << '\n';
                    return 1;
                }
                if ((arg == "-w") || (arg == "--width")) {
                    exp_width = x;
                } else if ((arg == "-l") || (arg == "--lines")) {
                    exp_height = x;
                } else  {                
                    exp_maxval = x;
                }
                i++; // Increment to get the right argument as the next argv[i].
            } else { // No argument to the option.
                  std::cerr << "Option requires a number." << std::endl;
                return 1;
            }  
        }

    }

    std::string upath;
    std::cout << "Enter path to .pgm files w/o whitespaces and w/o trailing '\\': ";
    std::cin >> upath;
    
    std::vector<std::size_t> avg(exp_width * exp_height, 0); //vector to create the averaged .pgm file from
    std::size_t lindex=0; //line index of the vector
    std::size_t count=0; //number of processed .pgm files
    std::string line; //string for iteration through the lines of the .pgm
    std::size_t found; //evaluate at which index substring was found in string
    pgm_header pgm_h; //struct for the header information
    std::string epath; //string for the .pgm file paths based on directory_iterator
    bool mmflag=0; //mismatch flag is set if grey values per line or number of lines do not match the header

    //Path validity check
    if (fs::is_directory(upath)){
        std::cout << "\nDirectory exists\n\n";
     
        for(auto const& dir_entry: fs::directory_iterator{upath})   {
            found = dir_entry.path().string().find(ext); //does contain .pgm?
            if (found!=std::string::npos)   {
                epath = dir_entry.path().string(); //get .pgm path from directory_iterator
                std::ifstream pgmfile (epath); //create input file stream object and open path
                pgm_h = F_ExtractHeaderInfo(pgmfile); //pass input stream object to function to get header information

                //no warning during header information extraction, all header information acc. to expectation
                if( pgm_h.wflag == 0 && pgm_h.magicn==exp_magicn && pgm_h.height==exp_height && pgm_h.width == exp_width) {   
                    for( std::size_t i=0; i<pgm_h.height; i++)    { 
                        //read the next data line from the pmgfile
                        std::getline(pgmfile, line);
                        if(!line.empty())   {
                            std::size_t n=0; 
                            std::stringstream stream(line); //create stringstream object
                            while(stream >> n)  {
                                avg.at(i*pgm_h.width + lindex) += n; //add up values of pixel coordinates from all files
                                lindex += 1;
                            }
                            if( lindex!=exp_width)  {
                                mmflag=1; //Warning: pixels per line do not match expectation
                            }
                            lindex = 0; //reset lindex after each image line is processed
                        } else  {
                            mmflag=1; //Warning: pixel lines do not match expectation
                        }
                    }
                    count +=1; //keep track of the so far processed .pgm files
                    std::cout <<"Processed file: \n"<< epath << "\n";
                }
                else    {
                    std::cout <<"Warning: The following .pgm file will not be used for averaging: \n"<< epath << "\n\n";
                }
            }
        }
        //are there any processed .pgm files?
        if(count > 0)   {
            //create new .pgm file under upath
            std::string result = "\\average.pgm";
            std::ofstream avgfile(upath.append(result)); //create output file stream object
            try {
                //write header
                avgfile <<"P2\n";
                avgfile << exp_width << " " << exp_height << "\n";
                avgfile << exp_maxval << "\n";
                for ( std::size_t h=0; h<exp_height; h++) {
                    for( std::size_t w=0; w<exp_width; w++)   {
                        //calculate average and write to file
                        avg.at(h*exp_width + w) = avg.at(h*exp_width + w) / count;
                        avgfile << avg.at(h*exp_width + w) << " ";
                    }
                    avgfile << "\n";
                }
                avgfile.close();
                if (!mmflag)    {
                    std::cout << "Result file successfully written" << '\n';   
                } else  {
                    std::cout << "Result file written\nWarning: Mismatch between header and grey values detected." << '\n';   
                }
            }
            catch(const std::exception& e)  {
                avgfile.close(); 
                std::cerr << e.what() << '\n';
            }
        }
        else    {  
            std::cout << "No .pgm files or no valid .pgm files in target directory.\n";
        }
    }
    else {
        std::cout << "Directory doesn't exist\n";
        std::cout <<  upath << "\n\n";
    }
    return 0;
}