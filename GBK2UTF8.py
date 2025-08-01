import os
import chardet
from pathlib import Path

def convert_gbk_to_utf8(file_path):
    """检测文件编码，如果是 GBK 则转换为 UTF-8"""
    try:
        # 读取文件二进制内容并检测编码
        with open(file_path, 'rb') as f:
            raw_data = f.read()
            encoding = chardet.detect(raw_data)['encoding']

        # 如果是 GBK 或 GB2312，转换为 UTF-8
        if encoding.lower() in ('gbk', 'gb2312', 'gb18030'):
            print(f"Converting {file_path} from {encoding} to UTF-8")
            with open(file_path, 'r', encoding=encoding) as f:
                content = f.read()
            with open(file_path, 'w', encoding='utf-8') as f:
                f.write(content)
        else:
            print(f"Skipping {file_path} (encoding: {encoding})")
    except Exception as e:
        print(f"Error processing {file_path}: {str(e)}")

def process_directory(directory):
    """递归遍历目录并处理文件"""
    for root, _, files in os.walk(directory):
        for file in files:
            file_path = os.path.join(root, file)
            convert_gbk_to_utf8(file_path)

if __name__ == "__main__":
    target_dir = input("请输入要处理的目录路径: ").strip()
    if os.path.isdir(target_dir):
        process_directory(target_dir)
        print("处理完成！")
    else:
        print("错误：指定的路径不是目录！")