
# Compile and Install

**Operating system**

Ubuntu 16.04. Not yet tested on other systems.

**Requirements**

[Boost](https://www.boost.org/) 1.62.0 or later

```
wget https://dl.bintray.com/boostorg/release/1.69.0/source/boost_1_69_0.tar.gz
tar -xzvf boost_1_69_0.tar.gz 
cd boost_1_69_0/
./bootstrap.sh 
sudo ./b2 install --build-type=complete --layout=versioned threading=multi -j 8
```

[libsecp256k1](https://github.com/libbitcoin/secp256k1)

```
git clone https://github.com/libbitcoin/secp256k1.git
cd secp256k1
./autogen.sh
./configure --enable-module-recovery
make
./tests
sudo make install
```

[libbitcoin](https://github.com/QuickBlock/libbitcoin)

You MUST use the QuickBlock/libbitcoin to install the bitcoin library. Or it may not be compatible.
```
cd libbitcoin
chmod +x autogen.sh
./autogen.sh 
./configure
make 
sudo make install
```

[glog](https://github.com/google/glog)

```
./autogen.sh
./configure
make
sudo make install
```


**Compilation**

```
mkdir bin
cd bin
cmake ..
make
```

