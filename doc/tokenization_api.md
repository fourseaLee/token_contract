# **Tokenization API**


---


注：以下都是post请求

- **issueToken**

1.参数说明：

| 参数       | 描述  | 类型|  限制字节  |
| :--------:   | :-----:  | :---: | :----:  |
| issuer_address     | 发行者的地址|string |   -     |
| contract_address        |   合约地址   |string |  -   |
| contract_content        |    合约内容哈希   | string |32  |
| contract_name | 合约名称 | string | 32 |
| asset_type       |    资产类型   | string |3  |
| asset_id      |    资产id   | string |32  |
| asset_qty        |    资产数量   | uint64_t |8  |


2.curl请求示例：

```
curl --data-binary '{"issuer_address":"qpfn7kcwm33ypzrtrhddgdehxwspar6mwgg95vrwpc","contract_address":"qr8l2pn00nexr8wxjcsl2qtwzkhht9lldqls4dj9p8","contract_content":"1","contract_name":"token","asset_type":"1","asset_id":"1","asset_qty":20}' -H 'content-type:text/plain;' http://192.168.0.12:8545/issueToken
```

3.返回结果：

```
{"code":0,"data":["d4dc9a1c5b0636c89511ea56c23d151c8286764a63ac823df42491981894afc7","d8bbe8f861b091325487f0dcef651a6d1f79a63108afff25659bb513ae02c01d","37e3a25067322878517d36c6e067a9334f5c403345aac3630d0f02ac6bb6e400","4fa66b095484aace50f51838881800b5fcc8e0abc95220811279d298459912b7"]}
```
- **transferToken**

1.参数说明：

| 参数       | 描述  | 类型|  限制字节  |
| :--------:   | :-----:  | :---: | :----:  |
| send_address     | 发送方地址|string |   -     |
| contract_address        |   合约地址   |string |  -   |
| recieve_address        |    接收方地址   | string |-  |
| currency_count       |    货币数量   | double |-  |
| asset_id      |    资产id   | string |32  |
| share_count        |    转移资产数量   | uint64_t |8  |



2.curl请求示例：

```
curl --data-binary '{"send_address":"qr5d7cthmrzejvr0nnhd86647lhu5egu3qxsuvrrgy","contract_address":"qree0fkfsld76nqgx6zw8txxtgxm0uzjgc2lc4fazd","recieve_address":"qrtnw4f4rr8tg5s3gdvgtl2zf00n5ur2tulu0hh8rm","currency_count":5,"asset_id":"1","share_count":5000}' -H 'content-type:text/plain;' http://192.168.0.18:8545/transferToken
```
3.返回结果：

```
{"code":0,"data":["d4dc9a1c5b0636c89511ea56c23d151c8286764a63ac823df42491981894afc7","d8bbe8f861b091325487f0dcef651a6d1f79a63108afff25659bb513ae02c01d"]}
```

- **queryToken**

1.参数说明：

| 参数       | 描述  | 类型|  限制字节  |
| :--------:   | :-----:  | :---: | :----:  |
| address     | 资产地址|string |   -     |
| contract        |   合约地址   |string |  -   |
| asset_id        |    资产id   | string |32  |


2.curl请求示例：

```
curl --data-binary '{"address":"qr8l2pn00nexr8wxjcsl2qtwzkhht9lldqls4dj9p8","contract":"qrtnw4f4rr8tg5s3gdvgtl2zf00n5ur2tulu0hh8rm","asset_id":"1"}' -H 'content-type:text/plain;' http://192.168.0.18:8545/queryToken
```
3.返回结果：

```
{"code":0,"data":1000000000}
```



