#!/usr/bin/env python
# -*- encoding: utf-8 -*-
"""
@copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights reserved.
            北京天智祥信息科技有限公司版权所有
@website: http://www.tianzhixiang.com.cn/
@author: dou li yang
@date: 2024-12-18
@file: 2DTask.py
@description: 2DTask实现
@version: v1.0.1 2024-12-18 dou li yang
"""
import os
import json
import time
import threading
from pathlib import Path
import websocket
import requests
import logging
import sys
from datetime import datetime
from concurrent.futures import ThreadPoolExecutor

# 设置日志格式
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)

# 根据是否为调试模式设置不同的路径和URL
if not hasattr(sys, 'gettrace') or sys.gettrace() is None:
    root = r"D:\TzxProject\Webs\dem\server\task"
else:
    root = r"\\192.168.0.9\Webs\dem\server\task"

proj = ""
plan = ""
svrr = ""
run_flag = True

WAIT = "Wait"
INVALID = "Invalid"
START = "Model initialized"
FINISH = "Model initialized and simulation complete"
EXTENSION = ".tzx_task"


def message_action(ws, message):
    logger.info(f"接受到信息: {message}")
    if message == WAIT:
        logger.info(f"系统忙,稍后计算: {proj} {plan}")
        global run_flag
        run_flag = True
    elif message == START:
        logger.info(f"开始计算: {proj} {plan}")
        run_flag = True
    elif message == INVALID:
        logger.info(f"无效任务, 移除任务: {proj} {plan}")
        remove_task()
        run_flag = True
    elif message == FINISH:
        logger.info(f"计算完成, 移除任务: {proj} {plan}")
        remove_task()
        run_flag = True


def latest_task():
    global proj, plan
    proj = ""
    plan = ""
    time_filename = {}
    sfp_p = Path(root)
    if not sfp_p.exists():
        return

    for item in sfp_p.iterdir():
        if item.is_dir() or item.suffix != EXTENSION:
            continue

        filename = item.stem
        time_filename[item.stat().st_mtime] = filename

    if not time_filename:
        return

    filename = time_filename[max(time_filename.keys())]
    parts = filename.split(".")
    if len(parts) < 2:
        return

    proj, plan = parts[0], parts[1]
    logger.info(f"检查到新任务: {proj} {plan}")


def remove_task():
    global proj, plan
    try:
        filename = f"{proj}.{plan}{EXTENSION}"
        file_path = Path(root) / filename
        file_path.unlink(missing_ok=True)
    except Exception as e:
        logger.error(f"移除任务失败: {filename}, 错误: {e}")

    proj = ""
    plan = ""


def main_loop():
    global svrr, url, run_flag
    while True:
        try:
            response = requests.get("http://192.168.0.9:3000/server/path")
            if response.status_code == 200:
                svrr = response.text.strip()
                logger.info(f"运行根目录: {svrr}")
            else:
                logger.error("获取运行根目录失败")
                time.sleep(5)
                continue
        except requests.RequestException as e:
            logger.error(f"获取运行根目录失败, 错误: {e}")
            time.sleep(5)
            continue

        url = "ws://192.168.0.9:80/ws1"

        try:
            ws = websocket.WebSocketApp(url, on_message=message_action)
            wst = threading.Thread(target=ws.run_forever)
            wst.daemon = True
            wst.start()

            while True:
                if not plan and not proj and run_flag:
                    latest_task()
                    if proj and plan:
                        msg = {
                            "argc": 2,
                            "command": "initialize_and_simulate",
                            "argv": [svrr, f"./{proj}/cfg/{plan}.cfg"]
                        }
                        jstr = json.dumps(msg)
                        logger.info(f"发送任务信息: {jstr}")

                        with ThreadPoolExecutor(max_workers=1) as executor:
                            future = executor.submit(ws.send, jstr)
                            send_result = future.result(timeout=5)

                        run_flag = False

                if not run_flag:
                    break

                time.sleep(10)

        except Exception as e:
            logger.error(f"连接或通信错误: {e}")
            time.sleep(10)

        time.sleep(5)


if __name__ == "__main__":
    import sys

    if len(sys.argv) > 1:
        root = sys.argv[1]
    if len(sys.argv) == 3:
        url = sys.argv[2]

    main_loop()
