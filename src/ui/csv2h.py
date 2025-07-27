import csv
import os
import shutil
from datetime import datetime

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


def csv_to_header(csv_filename, header_filename):
    # 备份现有的 .h 文件
    backup_file(header_filename)

    try:
        # 读取 CSV 文件
        with open(csv_filename, mode='r', encoding='utf-8') as csvfile:
            reader = csv.reader(csvfile)
            next(reader)  # 跳过标题行

            lines = []
            for row in reader:
                if len(row) == 3:
                    text_key = row[0].strip()
                    en_text = row[1].strip()
                    zh_text = row[2].strip()
                    # 格式化为头文件的格式
                    line = f'    {{"{text_key}", {{"{en_text}", "{zh_text}"}}}},'
                    lines.append(line)

        # 写入新的 .h 文件，添加头部和尾部
        with open(header_filename, mode='w', encoding='utf-8') as headerfile:
            # 写入头部
            headerfile.write('#pragma once\n')
            headerfile.write('#include <unordered_map>\n')
            headerfile.write('#include <string>\n\n')

            headerfile.write('struct TranslationEntry {\n')
            headerfile.write('    const char* english;\n')
            headerfile.write('    const char* chinese;\n')
            headerfile.write('};\n\n')

            # 写入翻译条目
            headerfile.write('static const std::unordered_map<std::string, TranslationEntry> g_translations = {\n')
            headerfile.write("\n".join(lines) + "\n")
            headerfile.write('};\n')

        print(f".h 文件已成功创建：{header_filename}")
    
    except Exception as e:
        print(f"处理 CSV 文件时发生错误: {e}")


# 例子：从 language.csv 文件生成 translations.h 文件
csv_filename = 'language.csv'  # 输入 CSV 文件路径
header_filename = 'translations.h'  # 输出 .h 文件路径
csv_to_header(csv_filename, header_filename)
