#pragma once

#include <iostream>
#include <filesystem>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <string>





#if __cplusplus < 201703L
    namespace fs = std::experimental::filesystem;
#else
    namespace fs = std::filesystem;
#endif

class File
{
public:

	static void mkdir(std::string& path);
	static void rmdir(std::string& path,bool remove_all);

	static bool isFile(std::string& path);
	static bool isDir(std::string& path);

	static bool read(std::string path, char *&content);
	static bool write(std::string path, char *&content,bool overwrite);

	static size_t read_mnist(std::string path,std::vector<std::vector<double>>&vec);
	static void read_mnist_label(std::string path, std::vector<__int8>&vec);

	

private:
	static int ReverseInt(int i);
};

inline void File::mkdir(std::string& path)
{
	if (isDir(path))
		return;
	if (std::count(path.begin(), path.end(), '/') > 0)
		fs::create_directories(path);
	else
		fs::create_directory(path);
}

inline void File::rmdir(std::string& path, bool remove_all)
{
	if (remove_all)
		fs::remove_all(path);
	else
		fs::remove(path);
}

inline bool File::isFile(std::string& path)
{
	struct stat buffer;   
	return (stat (path.c_str(), &buffer) == 0);
}

inline bool File::isDir(std::string& path)
{
	struct stat info;
	if (stat(path.c_str(), &info) != 0) {
		std::cout << "cannot access: "<<path <<std::endl;
		return false;
	}
	else if (info.st_mode & S_IFDIR)
		return false;
	else
		return true;
}

inline bool File::read(std::string path, char *& source)
{
	FILE *fp = fopen(path.c_str(), "rb");
	if (fp != NULL) {
		source = NULL;
		if (fseek(fp, 0L, SEEK_END) == 0) {
			long bufsize = ftell(fp);
			if (bufsize == -1) {}
			source = new char[bufsize + 1];
			if (fseek(fp, 0L, SEEK_SET) != 0) {}
			size_t newLen = fread(source, sizeof(char), bufsize, fp);
			if (ferror(fp) != 0) {
				fputs("Error reading file", stderr);
			}
			else {
				source[newLen++] = '\0';
			}
		}
		fclose(fp);
		return true;
	}
	else
		return false;
}

inline bool File::write(std::string path, char *& content, bool overwrite=true)
{
	FILE* fp;
	if (overwrite)
		fp = fopen(path.c_str(), "w");
	else
		fp = fopen(path.c_str(), "a");
    fwrite(content, sizeof(char),strlen(content), fp);
    fclose(fp);
    return 0;
}



inline size_t File::read_mnist(std::string path,std::vector<std::vector<double>>&vec)
{
	std::ifstream file(path,std::ifstream::binary);
	
	int number_of_images=0;
    if (file.is_open())
    {
        int magic_number=0;
       
        int n_rows=0;
        int n_cols=0;
        file.read((char*)&magic_number,sizeof(magic_number)); 
        magic_number= ReverseInt(magic_number);
        file.read((char*)&number_of_images,sizeof(number_of_images));
        number_of_images= ReverseInt(number_of_images);
        file.read((char*)&n_rows,sizeof(n_rows));
        n_rows= ReverseInt(n_rows);
        file.read((char*)&n_cols,sizeof(n_cols));
        n_cols= ReverseInt(n_cols);
        for(size_t i=0;i<number_of_images;++i)
        {
			std::vector<double>tp;
            for(int r=0;r<n_rows;++r)
            {
                for(int c=0;c<n_cols;++c)
                {
                    unsigned char temp=0;
                    file.read((char*)&temp,sizeof(temp));
					tp.push_back(((double)temp)/256.0);
					//std::cout << tp.back()<<" "<<std::endl;
                }
				//std::cout << std::endl;
            }
			vec.push_back(tp);
        }
    }
	return number_of_images;
}

inline void File::read_mnist_label(std::string path, std::vector<__int8>& vec)
{
	std::ifstream file (path, std::ios::binary);
    if (file.is_open())
    {
        int magic_number = 0;
        int number_of_images = 0;
        int n_rows = 0;
        int n_cols = 0;
        file.read((char*) &magic_number, sizeof(magic_number));
        magic_number = ReverseInt(magic_number);
        file.read((char*) &number_of_images,sizeof(number_of_images));
        number_of_images = ReverseInt(number_of_images);
        for(int i = 0; i < number_of_images; ++i)
        {
            unsigned char temp = 0;
            file.read((char*) &temp, sizeof(temp));
            vec[i]= (__int8)temp;
        }
    }
	return;
}



inline int File::ReverseInt(int i)
{
	unsigned char c1, c2, c3, c4;

    c1 = i & 255;
    c2 = (i >> 8) & 255;
    c3 = (i >> 16) & 255;
    c4 = (i >> 24) & 255;

    return ((int)c1 << 24) + ((int)c2 << 16) + ((int)c3 << 8) + c4;
}
