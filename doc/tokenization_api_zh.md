# **Tokenization API**
本项目搭载contractd,scanblock,contractcli 三个可执行程序完成基于bch上发行token的方案。
contractd是api层：
    完成合约方对外的api接口调用
scanblock是存储层
    完成合约方搭建对外扩展应用和提高contractd响应的速度
contractcli是客户端
    完成用户的线下操作

如果使用方是合约方：
    需要配置全节点，数据库（我们这里使用的mysql），kafka，
    完成conf下的conf.json 配置，运行contractd和scanblock

现在tokend提供给对外接口：
    
    获取未花费的交易： getUtxo
    调用举例：
    curl   --data-binary '{"address":"mow6u7mUyQEJup65DbkqLK2ZRaAcibrgA5", "value":"0.00050000"}' http://192.168.0.26:6666/GetUtxo
    结果：
    {"code":0,"data":[["364af8dffccdbfdfa9be4b2527dacbd35575f514828c342edb604b50a4c8b4d9",0,"0.000050"],["c864ca7deabda33dc1bf07f5798ae84834137b9ad32f307dfff1ad80d0a76898",0,"0.000120"],["d1b959c263fd1ebafdf653bbeed43b5b21d9f440ecdfe3c2345fdac6bbcca489",1,"0.000110"],["9e847433ae44f92638e97217dc2cfb81e6a6e555dd97fd403a0b12944a877183",0,"0.099940"]]}
    
    广播交易： SendTransaction
    调用举例：
    curl   --data-binary  '{"hex":"02000000012ab3215b1d0e6 .....7cd6220a03e79d87e5ace71b85761","utxo":
    [["364af8dffccdbfdfa9be4b2527dacbd35575f514828c342edb604b50a4c8b4d9",0,"0.000050"]... ]] http://192.168.0.26:6666/GetUtxo
    结果：
    {"code":0,"data":"2df038ba049ce50f0d951f8a6f67f59deff754c7c148654fb3f6f433122b3bf3"}
    
    查询资产：QueryBalance
    调用举例：
    curl --data-binary '{"address":"qr8l2pn00nexr8wxjcsl2qtwzkhht9lldqls4dj9p8","contract":"qrtnw4f4rr8tg5s3gdvgtl2zf00n5ur2tulu0hh8rm","asset_id":"1"}' -H 'content-type:text/plain;' http://192.168.0.26:6666/QueryBalance
    结果：
    {"code":0,"data":1000}
    

用户线下交互contractcli命令行解析

支持用户输入参数和输入json文件作为参数,参数必须是一个json对象
相关参数的模板可以比照 params/下的json文件
支持的用户命令有：
    Help 
调用举例
    ./contractcli -f Help -p {"function":"GetUtxo"} -u http://192.168.0.26:6666
    
    GetUtxo
调用举例：
    ./contractcli -f GetUtxo -j  get_utxo.json -u http://192.168.0.26:6666
    
    ContractOffer
调用举例：
    ./contractcli -f ContractOffer -j  contract_offer.json -u http://192.168.0.26:6666
    
    AssetDefinition
调用举例：
    ./contractcli -f AssetDefinition -j  asset_definition.json -u http://192.168.0.26:6666

    TokenSend
调用举例：
    ./contractcli -f TokenSend -j  token_send.json -u http://192.168.0.26:6666

    TokenRecieve
调用举例：
    ./contractcli -f TokenRecieve -j  token_recieve.json -u http://192.168.0.26:6666


