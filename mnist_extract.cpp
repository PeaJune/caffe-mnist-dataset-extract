#include <iostream>
#include <vector>
#include <fstream>

using namespace std;

#pragma pack(1)

typedef struct
{
    short type;
    int size;
    short reserved1;
    short reserved2;
    int offset;
} BMPHeader;

typedef struct
{
    int size;
    int width;
    int height;
    short planes;
    short bitsPerPixel;
    unsigned compression;
    unsigned imageSize;
    int xPelsPerMeter;
    int yPelsPerMeter;
    int clrUsed;
    int clrImportant;

} BMPInfoHeader;

typedef struct
{
    uint8_t rgbBlue;
    uint8_t rgbGreen;
    uint8_t rgbRed;
    uint8_t rgbReserved;
} ColorTable;

#pragma pack()

int saveBMPFile(uint8_t *pu8BgrBuf, int width, int height, const char *name)
{
    BMPHeader hdr;
    BMPInfoHeader infoHdr;
    ColorTable color[256] = {0};
    int i;
    FILE *fp = NULL;

    fp = fopen(name, "wb");
    if (NULL == fp)
    {
        printf("saveBMPFile: Err: Open!\n");
        return (-1);
    }

    infoHdr.size = sizeof(BMPInfoHeader);
    infoHdr.width = width;
    infoHdr.height = height;
    infoHdr.planes = 1;
    infoHdr.bitsPerPixel = 8;
    infoHdr.compression = 0;
    infoHdr.imageSize = width * height;
    infoHdr.xPelsPerMeter = 0;
    infoHdr.yPelsPerMeter = 0;
    infoHdr.clrUsed = 0;
    infoHdr.clrImportant = 0;

    hdr.type = 0x4D42;
    hdr.size = 54 + infoHdr.imageSize + sizeof(color);
    hdr.reserved1 = 0;
    hdr.reserved2 = 0;
    hdr.offset = 54;

    for (int i = 0; i < 256; i++) {
        color[i].rgbBlue = i;
        color[i].rgbGreen = i;
        color[i].rgbRed = i;
    }

    fwrite(&hdr, sizeof(hdr), 1, fp);
    fwrite(&infoHdr, sizeof(infoHdr), 1, fp);
    fwrite(&color, sizeof(color), 1, fp);
    for (i = 0; i < height; i++)
    {
        fwrite(pu8BgrBuf + (height - 1 - i) * width, sizeof(unsigned char), width , fp);
    }
    fclose(fp);
    fp = NULL;

    return 0;
}

uint32_t swap_endian(uint32_t val) {
    val = ((val<< 8) & 0xff00ff00) | ((val>>8) & 0xff00ff);
    return (val>>16) | (val<<16);
}

int main(int argc, char** argv) {
	if (argc < 2) {
		cout<<"usage: ./mnist_extract path/t10k-images-idx3-ubyte"<<endl;
		return -1;
	}

    ifstream image_file(argv[1]);
    if (!image_file) {
        cerr<<"open "<<argv[1]<<" failed."<<endl;
        return 0;
    }
	/*
	* 图片存储文件格式
	* struct{
	*	int32_t magic;	//魔数, 等于2051
	*	int32_t items;  //总的条数
	*	int32_t rows;	//行
	*	int32_t cols;	//列
	*	uint8_t* data;	//图像数据,每张图像数据大小为 rows*cols
	*}
	*/
	/*
	* 标签存储文件格式
	* struct {
	*	int32_t magic; //魔数, 等于2049
	*	int32_t items; //总的条数
	*	uint8_t *label; //label 数据, 每个label大小为1 byte
	*}
	*/

    uint32_t magic;
    uint32_t num_items;
    uint32_t rows;
    uint32_t cols;

    image_file.read(reinterpret_cast<char*>(&magic), 4);
    magic = swap_endian(magic);
    cout<<"image magic is :"<<magic<<endl;
    if (magic != 2051)
    {
    	cout<<"file may be corrupted, check input file."<<endl;
	image_file.close();
	return -1;
    }

    image_file.read(reinterpret_cast<char*>(&num_items), 4);
    num_items = swap_endian(num_items);
    cout<<"image numbers: "<<num_items<<endl;

    image_file.read(reinterpret_cast<char*>(&rows), 4);
    image_file.read(reinterpret_cast<char*>(&cols), 4);
    rows = swap_endian(rows);
    cols = swap_endian(cols);
    cout<<"images row x cols = "<<rows<<" x "<<cols<<endl;
    std::vector<uint8_t> data(rows * cols);
    
    for(int i = 0; i < num_items; ++i) {
        string file_name = std::to_string(i) + ".bmp";
        image_file.read(reinterpret_cast<char*>(&data.front()), rows * cols);
        saveBMPFile(&data.front(), rows, cols, file_name.c_str());
        cout<<"saved "<<file_name<<endl;
    }
    image_file.close();

    return 0;
}
