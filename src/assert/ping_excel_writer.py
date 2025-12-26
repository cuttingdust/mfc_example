#!/usr/bin/env python3
"""
Excel写入模块
"""

import pandas as pd
from datetime import datetime
import threading
import os

# 线程锁，确保Excel写入安全
_excel_lock = threading.RLock()
_data_buffer = []  # 数据缓冲区
_excel_initialized = False
_dataframe = None

def init_excel_file():
    """初始化Excel文件"""
    global _dataframe, _excel_initialized
    
    with _excel_lock:
        if not _excel_initialized:
            # 检查文件是否存在
            filename = "ping_results.xlsx"
            if os.path.exists(filename):
                try:
                    _dataframe = pd.read_excel(filename)
                except Exception as e:
                    print(f"Warning: Could not read existing Excel file: {e}")
                    _dataframe = pd.DataFrame(columns=[
                        'Timestamp', 'IP Address', 'Bytes', 'Time (ms)', 
                        'TTL', 'Status', 'Thread ID'
                    ])
            else:
                _dataframe = pd.DataFrame(columns=[
                    'Timestamp', 'IP Address', 'Bytes', 'Time (ms)', 
                    'TTL', 'Status', 'Thread ID'
                ])
            
            _excel_initialized = True
            print(f"Excel file initialized: {filename}")

def write_ping_result(ip, bytes_sent, ping_time, ttl, success):
    """
    写入单条Ping结果到Excel
    由C++直接调用
    
    Args:
        ip: IP地址
        bytes_sent: 发送的字节数
        ping_time: 往返时间(ms)
        ttl: TTL值
        success: 是否成功
    """
    global _data_buffer
    
    # 延迟初始化
    if not _excel_initialized:
        init_excel_file()
    
    # 获取当前线程ID
    thread_id = threading.current_thread().ident
    
    # 创建数据记录
    record = {
        'Timestamp': datetime.now().strftime('%Y-%m-%d %H:%M:%S.%f')[:-3],
        'IP Address': ip,
        'Bytes': int(bytes_sent),
        'Time (ms)': int(ping_time),
        'TTL': int(ttl),
        'Status': 'Success' if success else 'Failed',
        'Thread ID': thread_id
    }
    
    # 添加到缓冲区
    with _excel_lock:
        _data_buffer.append(record)
        
        # 缓冲区达到一定数量或超时后写入文件
        if len(_data_buffer) >= 10:  # 每10条记录写入一次
            flush_to_excel()

def write_batch_results(records_list):
    """
    批量写入Ping结果（性能优化版本）
    
    Args:
        records_list: 记录列表，每个记录是字典
    """
    global _data_buffer
    
    if not _excel_initialized:
        init_excel_file()
    
    with _excel_lock:
        # 转换Python对象为字典
        for py_record in records_list:
            record = {
                'Timestamp': datetime.now().strftime('%Y-%m-%d %H:%M:%S.%f')[:-3],
                'IP Address': str(py_record.get('ip', '')),
                'Bytes': int(py_record.get('bytes', 0)),
                'Time (ms)': int(py_record.get('time', 0)),
                'TTL': int(py_record.get('ttl', 0)),
                'Status': 'Success' if py_record.get('success', False) else 'Failed',
                'Thread ID': threading.current_thread().ident
            }
            _data_buffer.append(record)
        
        if len(_data_buffer) >= 5:
            flush_to_excel()

def flush_to_excel():
    """将缓冲区数据写入Excel文件"""
    global _data_buffer, _dataframe
    
    if not _data_buffer:
        return
    
    with _excel_lock:
        try:
            # 创建临时DataFrame
            temp_df = pd.DataFrame(_data_buffer)
            
            # 合并到主DataFrame
            if _dataframe is None:
                _dataframe = temp_df
            else:
                _dataframe = pd.concat([_dataframe, temp_df], ignore_index=True)
            
            # 写入Excel文件
            with pd.ExcelWriter('ping_results.xlsx', engine='openpyxl') as writer:
                _dataframe.to_excel(writer, index=False, sheet_name='Ping Results')
                
                # 添加汇总sheet
                if len(_dataframe) > 0:
                    summary = _dataframe.groupby(['IP Address', 'Status']).agg({
                        'Time (ms)': ['count', 'mean', 'min', 'max'],
                        'TTL': 'mean'
                    }).round(2)
                    
                    summary.columns = ['Count', 'Avg Time', 'Min Time', 'Max Time', 'Avg TTL']
                    summary.reset_index(inplace=True)
                    summary.to_excel(writer, index=False, sheet_name='Summary')
                
                # 自动调整列宽
                worksheet = writer.sheets['Ping Results']
                for column in worksheet.columns:
                    max_length = 0
                    column_letter = column[0].column_letter
                    for cell in column:
                        try:
                            if len(str(cell.value)) > max_length:
                                max_length = len(str(cell.value))
                        except:
                            pass
                    adjusted_width = min(max_length + 2, 30)
                    worksheet.column_dimensions[column_letter].width = adjusted_width
            
            print(f"Excel updated: {len(_data_buffer)} records written")
            
            # 清空缓冲区
            _data_buffer.clear()
            
        except Exception as e:
            print(f"Error writing to Excel: {e}")
            # 保存错误数据到文本文件作为备份
            try:
                with open('ping_backup.txt', 'a') as f:
                    for record in _data_buffer:
                        f.write(str(record) + '\n')
                print("Data backed up to ping_backup.txt")
                _data_buffer.clear()
            except:
                pass

def save_and_close():
    """保存所有剩余数据并关闭文件"""
    print("Saving all remaining data to Excel...")
    flush_to_excel()
    
    if _dataframe is not None:
        print(f"Total records saved: {len(_dataframe)}")
        if len(_dataframe) > 0:
            success_rate = (_dataframe['Status'] == 'Success').mean() * 100
            avg_time = _dataframe[_dataframe['Status'] == 'Success']['Time (ms)'].mean()
            print(f"Success rate: {success_rate:.2f}%")
            print(f"Average ping time: {avg_time:.2f} ms")
    
    print("Excel file saved successfully.")

# 模块初始化时设置
if __name__ == "__main__":
    # 测试代码
    print("ping_excel_writer module loaded")
    init_excel_file()
else:
    # 被导入时初始化
    init_excel_file()