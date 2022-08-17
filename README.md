# 描述 
    因为想找一个手写的数据库，网上现成的要不是收费的，要不是各种限制和条件，无奈只能自己写一个解压的程序了
    此程序没有什么依赖，仅需要g++ 编译即可
# 功能
    提取mnist dataset图片，并保存为bmp
# 编译
    g++ mnist_extract.cpp -o mnist_extract
# 运行
    数据集下载: ./data/mnist/get_mnist.sh
    ./mnist_extract ./data/mnist/t10k-images-idx3-ubyte

