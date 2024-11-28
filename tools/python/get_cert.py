#!/usr/bin/env python
# -*- encoding: utf-8 -*-
"""
@copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights reserved.
            北京天智祥信息科技有限公司版权所有
@website: http://www.tianzhixiang.com.cn/
@author: dou li yang
@date: 2024-11-25
@file: get_cert.py
@description: get_cert实现
@version: v1.0.1 2024-11-25 dou li yang
"""


import requests
import ssl
import socket

def get_ssl_certificate(hostname, port=465):
    context = ssl.create_default_context()
    with socket.create_connection((hostname, port)) as sock:
        with context.wrap_socket(sock, server_hostname=hostname) as ssock:
            cert = ssock.getpeercert()
            return cert

hostname = 'smtp.qq.com'
port = 465  # SMTP over SSL/TLS port

certificate = get_ssl_certificate(hostname, port)
print(certificate)