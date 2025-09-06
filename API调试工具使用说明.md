# API调试工具使用说明

## 功能概述

更新后的API调试工具现在支持认证令牌，可以方便地测试需要身份验证的API接口。

## 主要功能

### 1. 预设API支持
- **登录API**: 自动配置登录接口和参数
- **获取用户信息API**: 自动配置需要认证的接口
- **自定义API**: 手动输入任意API地址

### 2. 认证令牌支持
- 支持Bearer令牌认证
- 自动添加"Bearer "前缀（如果未包含）
- 令牌输入框支持密码模式（隐藏显示）
- 自动保存和恢复令牌

### 3. 参数管理
- 动态添加/删除请求参数
- 支持GET和POST请求参数
- 自动填充预设API的参数

## 使用步骤

### 步骤1: 获取登录令牌

1. 在"预设API"下拉框中选择"登录 - POST /test/face-api/desktop/user/login"
2. 系统会自动填充：
   - URL: `https://www.lihevitality.com/test/face-api/desktop/user/login`
   - 方法: POST
   - 参数: userName=it1, password=123456
3. 点击"发送请求"
4. 在响应结果中找到`accessToken`字段，复制其值

### 步骤2: 使用令牌获取用户信息

1. 在"预设API"下拉框中选择"获取用户信息 - GET /test/face-api/desktop/user/getInfo"
2. 系统会自动填充：
   - URL: `https://www.lihevitality.com/test/face-api/desktop/user/getInfo`
   - 方法: GET
3. 在"认证令牌"输入框中粘贴步骤1获取的`accessToken`值
4. 点击"发送请求"

### 步骤3: 自定义API测试

1. 在"预设API"下拉框中选择"自定义API"
2. 手动输入API地址
3. 选择请求方法
4. 添加必要的参数
5. 输入认证令牌（如需要）
6. 点击"发送请求"

## 令牌格式说明

### 完整格式
```
Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...
```

### 简化格式
```
eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...
```
*注意：如果只输入令牌值，系统会自动添加"Bearer "前缀*

## 响应状态码说明

- **200**: 请求成功
- **401**: 认证失败（令牌无效或过期）
- **403**: 权限不足
- **404**: 接口不存在

## 常见问题

### Q: 为什么获取用户信息时返回401错误？
A: 请确保：
1. 已正确获取登录令牌
2. 令牌未过期
3. 令牌格式正确（包含Bearer前缀）

### Q: 如何知道令牌是否过期？
A: 查看登录响应中的`expiresIn`字段，单位为秒。如果令牌过期，需要重新登录获取新令牌。

### Q: 可以保存多个不同的令牌吗？
A: 当前版本只保存一个令牌。如果需要测试不同用户的API，请手动更换令牌。

## 技术实现

- 使用QNetworkAccessManager发送HTTP请求
- 支持HTTPS和SSL证书验证
- 自动格式化JSON响应
- 使用QSettings保存用户配置
- 支持多种HTTP方法（GET, POST, PUT, DELETE, PATCH）

## 注意事项

1. 令牌具有时效性，过期后需要重新获取
2. 请妥善保管您的登录凭据
3. 调试工具仅用于开发和测试，请勿在生产环境中使用
4. 所有设置会在关闭窗口时自动保存
