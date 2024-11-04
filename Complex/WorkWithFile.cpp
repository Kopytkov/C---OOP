#include <fstream>
#include <iostream>
#include <vector>
#include <cstdint>
#include "ComplexClass.hpp"


#pragma pack(push, 1)
struct BFI
{
    short Signature;
    unsigned int FileSize;
    unsigned int Reserved;
    unsigned int DataOffset;
};

struct InfoHeader
{
    unsigned int Size;
    unsigned int Width;
    unsigned int Height;
    unsigned short Planes;
    unsigned short BitCount;
    unsigned int Compression;
    unsigned int ImageSize;
    unsigned int XpixelsPerM;
    unsigned int YpixelsPerM;
    unsigned int ColorsUsed;
    unsigned int ColorsImportant;

};

struct BMP
{
    BFI bf;
    InfoHeader ih;
};

struct RGB {
    uint8_t r, g, b;
};

struct BMPImage
{
    BMP bmp;
    std::vector<unsigned char> dib;

    // Default конструктор
    BMPImage() = default;

    // Конструктор для создания черного изображения
    BMPImage(uint32_t w, uint32_t h) {
        bmp.bf.Signature = 0x4D42;  // "BM" в ASCII
        if (w % 4 != 0){
            uint32_t w1 = 3 * w + (4 - (w*3)%4);
            bmp.bf.FileSize = sizeof(BMP) + w1 * h;
            bmp.ih.ImageSize = w1 * h;
        }
        else {
            bmp.bf.FileSize = sizeof(BMP) + 3 * w * h;
            bmp.ih.ImageSize = w * h * 3;
        };
        bmp.bf.Reserved = 0;
        bmp.bf.DataOffset = sizeof(BMP);

        bmp.ih.Size = sizeof(InfoHeader);
        bmp.ih.Width = w;
        bmp.ih.Height = h;
        bmp.ih.Planes = 1;
        bmp.ih.BitCount = 24;
        bmp.ih.Compression = 0;
        bmp.ih.XpixelsPerM = 0;
        bmp.ih.YpixelsPerM = 0;
        bmp.ih.ColorsUsed = 0;
        bmp.ih.ColorsImportant = 0;

        // Заполняем изображение черным цветом
        dib.resize(bmp.ih.ImageSize, 0);
    }

    void Read(std::fstream &file){
        file.read(reinterpret_cast <char*>(&bmp), sizeof(BMP));
        dib.resize(bmp.ih.ImageSize);
        file.read(reinterpret_cast <char*>(dib.data()), dib.size());
    };

    RGB GetPixel(uint32_t x, uint32_t y) const {
        const uint32_t line_bytes = bmp.ih.ImageSize / bmp.ih.Height;
        RGB color;
        color.b = dib[3 * x + line_bytes * y + 0];
        color.g = dib[3 * x + line_bytes * y + 1];
        color.r = dib[3 * x + line_bytes * y + 2];
        return color;
    }

    void SetPixel(uint32_t x, uint32_t y, const RGB& color) {
        const uint32_t line_bytes = bmp.ih.ImageSize / bmp.ih.Height;
        dib[3 * x + line_bytes * y + 0] = color.b;
        dib[3 * x + line_bytes * y + 1] = color.g;
        dib[3 * x + line_bytes * y + 2] = color.r;
    }

    void write(std::ofstream& outfile) const {
        outfile.write(reinterpret_cast<const char*>(&bmp), sizeof(BMP));
        outfile.write(reinterpret_cast<const char*>(dib.data()), dib.size());
    }
};
#pragma pack(pop)

Complex toC(uint32_t x, uint32_t y, uint32_t w, uint32_t h){
    float xComplex = 3 * (x / float(w)) - 2;
    float yComplex = 2 * (y / float(h)) - 1;
    Complex z(xComplex, yComplex);
    return z;
}

uint32_t M(const Complex c){
    uint32_t n = 0;
    Complex z = c;
    while (z.len() < 4){
        z = z * z + c;
        n++;
        if (n > 255){
            return 0;
        }
    }
    return n;
}

RGB GetPaletteColor(uint32_t n) {

    // Определение основных цветов для палитры
    // const std::vector<std::pair<int, RGB>> palette = {
    //     {0,   {0, 0, 0}},      // Черный
    //     {120, {255, 255, 0}},  // Желтый
    //     {160, {255, 0, 0}},    // Красный
    //     {255, {0, 0, 255}}     // Синий
    // };
    // const std::vector<std::pair<int, RGB>> palette = {
    //     {0,    {0, 0, 0}},      // Черный
    //     {36,   {75, 0, 130}},   // Индиго
    //     {72,   {0, 0, 255}},    // Синий
    //     {108,  {0, 255, 0}},    // Зеленый
    //     {144,  {255, 255, 0}},  // Желтый
    //     {180,  {255, 165, 0}},  // Оранжевый
    //     {216,  {255, 0, 0}},    // Красный
    //     {255,  {139, 0, 255}}   // Фиолетовый
    // };
    const std::vector<std::pair<int, RGB>> palette = {
        {0,    {0, 0, 0}},      // Черный
        {30,   {0, 0, 128}},    // Темно-синий
        {80,   {0, 0, 255}},    // Ярко-синий
        {150,  {255, 255, 255}},// Белый
        {200,  {255, 165, 0}},  // Оранжевый
        {230,  {255, 255, 0}},  // Желтый
        {255,  {0, 0, 0}}       // Черный
    };

    // Найдем, между какими отметками находится `n`
    for (size_t i = 0; i < palette.size() - 1; ++i) {
        if (n >= palette[i].first && n <= palette[i + 1].first) {
            // Определение весов для линейной интерполяции
            float t = float(n - palette[i].first) / (palette[i + 1].first - palette[i].first);
            
            // Линейная интерполяция по каждому каналу (r, g, b)
            uint8_t r = palette[i].second.r + t * (palette[i + 1].second.r - palette[i].second.r);
            uint8_t g = palette[i].second.g + t * (palette[i + 1].second.g - palette[i].second.g);
            uint8_t b = palette[i].second.b + t * (palette[i + 1].second.b - palette[i].second.b);
            
            return {r, g, b};
        }
    }

    // Если `n` выходит за границы (0 или 255), вернем соответствующий крайний цвет
    return n <= 0 ? palette.front().second : palette.back().second;
}

void Calc (BMPImage& bmpImage){
    uint32_t w = bmpImage.bmp.ih.Width;
    uint32_t h = bmpImage.bmp.ih.Height;
    for (uint32_t x = 0; x < w; x++){
        for (uint32_t y = 0; y < h; y++){
            Complex c = toC(x, y, w, h);
            uint32_t n = M(c);
            RGB color = GetPaletteColor(n*10);
            bmpImage.SetPixel(x, y, color);

            // RGB color = {
            //     static_cast<uint8_t>(n * 15), 
            //     static_cast<uint8_t>(n * 15), 
            //     static_cast<uint8_t>(n * 15)
            // };
            // RGB color;
            // color.r = color.b = color.b = n * 15;
            bmpImage.SetPixel(x, y, color);
        }
    }
}



int main(){
    // char c1, c2;
    // unsigned int fsize;
    // BFI bf;
    // InfoHeader ih;
    // BMP bmp;
    // BMPImage bmpimage;
    

    // std::ifstream fileName("C:\\Users\\suren\\.vscode\\C++ OOP\\filicheck.bmp", std::ios::binary);
    // // fileName.read(&c1, 1);
    // // fileName.read(&c2, 1);
    // // fileName.read(reinterpret_cast <char*>(&fsize), 4);
    // fileName.read(reinterpret_cast <char*>(&bf), sizeof(BFI));
    // fileName.read(reinterpret_cast <char*>(&ih), sizeof(InfoHeader));
    // fileName.read(reinterpret_cast <char*>(&bmp), sizeof(BMP));
    // // std::cout << bf << std::endl;


    // // std::cout << c1 << std::endl << c2 << std::endl << fsize << std::endl;
    // std::cout << "Size BFI = " << sizeof(BFI) << std::endl 
    // << "Size InfoHeader = " << sizeof(InfoHeader) << std::endl
    // << "Size BMP = " << sizeof(BMP) << std::endl;
    // // << "Size FileName = " << sizeof(fileName) << std::endl;

    BMPImage bmpimageOld;
    
    std::fstream file("C:\\Users\\suren\\.vscode\\C++ OOP\\Complex\\filicheck.bmp", std::ios::binary | std::ios::in);
    if (!file.is_open()) {
        std::cerr << "Error: could not open file." << std::endl;
        return 1;
    }
    bmpimageOld.Read(file);
    file.close();
    // Сохраняем изменения в новый файл
    Calc(bmpimageOld);
    std::ofstream outfileOld("C:\\Users\\suren\\.vscode\\C++ OOP\\Complex\\outputCalcOld.bmp", std::ios::binary);
    bmpimageOld.write(outfileOld);
    outfileOld.close();



    // Создаем изображение
    BMPImage bmpimage(1200, 900);

    // Вычисляем фрактал и заполняем изображение
    Calc(bmpimage);

    // Сохраняем результат в файл
    std::ofstream outfile("C:\\Users\\suren\\.vscode\\C++ OOP\\Complex\\outputCalc.bmp", std::ios::binary);
    if (!outfile) {
        std::cerr << "Error: could not open file for writing." << std::endl;
        return 1;
    }
    bmpimage.write(outfile);
    outfile.close();

    
    // // Тестируем получение пикселя
    // RGB pixel = bmpimage.GetPixel(0, 0);
    // std::cout << "RGB at (0, 0): (" << (int)pixel.r << ", " << (int)pixel.g << ", " << (int)pixel.b << ")" << std::endl;

    // // Тестируем изменение пикселя
    // bmpimage.SetPixel(0, 0, {255, 0, 0});  // Устанавливаем красный цвет для пикселя (0, 0)

    return 0;

}