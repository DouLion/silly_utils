#!/usr/bin/env python
# -*- encoding: utf-8 -*-
"""
@copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights reserved.
            北京天智祥信息科技有限公司版权所有
@website: http://www.tianzhixiang.com.cn/
@author: dou li yang
@date: 2024-11-25
@file: test_smtp.py
@description: test_smtp实现
@version: v1.0.1 2024-11-25 dou li yang
"""

import smtplib
from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart
from email.mime.base import MIMEBase
from email import encoders

# 邮件配置
smtp_server = 'smtp.qq.com'
smtp_port = 465  # SSL 端口
# smtp_port = 25  # 普通 端口
username = '2292405399@qq.com'
password = 'fevvtczwqnvjdicc'
sender = '2292405399@qq.com'
receiver = '734546831@qq.com'
subject = '测试python发送'
body = '测试使用python发送SMTP邮件.'

# 创建消息对象
msg = MIMEMultipart()
msg['From'] = sender
msg['To'] = receiver
msg['Subject'] = subject

"""
# 添加正文
msg.attach(MIMEText(body, 'plain'))

# 添加附件（可选）
filename = 'example.txt'
attachment = open(filename, 'rb')

part = MIMEBase('application', 'octet-stream')
part.set_payload(attachment.read())
encoders.encode_base64(part)
part.add_header('Content-Disposition', f'attachment; filename={filename}')
msg.attach(part)
"""
# 连接到SMTP服务器并发送邮件
try:
    server = smtplib.SMTP_SSL(smtp_server, smtp_port)
    # server = smtplib.SMTP(smtp_server, smtp_port)
    server.set_debuglevel(1)
    server.login(username, password)
    server.sendmail(sender, receiver, msg.as_string())
    print("Email sent successfully!")
except Exception as e:
    print(f"Failed to send email: {e}")
finally:
    server.quit()