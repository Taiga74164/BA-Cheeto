import os
import shutil
from datetime import datetime
import re
import csv

def backup_file(file_path):
    """备份现有文件，使用当前时间戳"""
    if not os.path.exists(file_path):
        print(f"文件 {file_path} 不存在，无法备份。")
        return
    
    # 获取文件的名称和扩展名
    base_name, ext = os.path.splitext(file_path)
    
    # 获取当前时间戳
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    
    # 创建备份文件名
    backup_file_path = f"{base_name}_backup_{timestamp}{ext}"
    
    try:
        # 复制文件并重命名为备份文件
        shutil.copy(file_path, backup_file_path)
        print(f"文件已备份为 {backup_file_path}")
    except Exception as e:
        print(f"备份文件时发生错误: {e}")


def header_to_csv(header_filename, csv_filename):
    # 备份现有的 .h 文件和 .csv 文件
    backup_file(header_filename)
    
    try:
        with open(header_filename, mode='r', encoding='utf-8') as headerfile:
            content = headerfile.read()

        # 使用正则表达式提取英文、中文和附加字段
        # 假设每行有一个条目格式： {"English Text", {"English Text", "Chinese Text", "Other Field"}}
        pattern = r'"(.*?)",\s*{\s*"(.*?)",\s*"(.*?)"\s*}'
        matches = re.findall(pattern, content)

        # 如果没有找到匹配的条目
        if not matches:
            print("无法从头文件中提取翻译条目。")
            return
        
        # 写入 CSV 文件
        with open(csv_filename, mode='w', newline='', encoding='utf-8') as csvfile:
            writer = csv.writer(csvfile)
            writer.writerow(["TEXT_KEY", "EN", "zh-CN"])  # 写入标题行
            for match in matches:
                english_text = match[0]
                chinese_text = match[1]
                other_field = match[2]  # 假设第三列为附加字段
                writer.writerow([english_text, chinese_text, other_field])

        print(f"CSV 文件已成功创建：{csv_filename}")
    
    except Exception as e:
        print(f"处理头文件时发生错误: {e}")

# 例子：从 translations.h 文件反转回 language.csv 文件
header_filename = 'translations.h'  # 输入头文件路径
csv_filename = 'language_reversed.csv'  # 输出 CSV 文件路径
header_to_csv(header_filename, csv_filename)
