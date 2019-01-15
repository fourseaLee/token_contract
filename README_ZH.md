这是一个基于blockchain的智能合约项目
现如今这个项目只在Ubuntu上编译测试通过

在使用这个项目的时候请先初始化数据库（执行doc/smart_contract.sql），这边使用的是mysql数据库。 并且配置好conf目录下的相关配置文件。

这个项目的依赖的第三库有 glog  pthread  mysqlclient  boost_system boost_program_options bitcoin  secp256k1
具体编译依赖请看文档"build-unix.md"

请将项目编译到bin目录下
cd  bin/ && cmake ../ && make
编译完成会生成contractd,scanblock,contractcli三个可执行文件
contractd是一个http服务器提供对外API,具体可以查看"doc/tokenization_api.md"
scanblock是一个区块同步的存储层，用于同步链上相关数据到数据库。
contractcli是一个客户端工具，用于普通用户和合约方交互

在linux平台下直接运行./contractd -l "日志相关配置文件的路径"  -c "程序运行的基本的配置文件的路径"

依照相应方法安装即可
以后相关详细解释请看doc目录下文件
