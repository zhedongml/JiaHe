import json
import pandas as pd
from pathlib import Path
import os

def normalize_number(x):
    """如果是浮点数但没有小数部分，转为 int"""
    if isinstance(x, float) and x.is_integer():
        return int(x)
    return x


def is_file_in_use(filepath: str) -> bool:
    """检测文件是否被占用"""
    if not os.path.exists(filepath):
        return False
    try:
        with open(filepath, "a"):
            return False
    except PermissionError:
        return True
    

def expand_logheaders(json_path: str, csv_path: Path):
    
    if is_file_in_use(str(csv_path)):
        print(f"⚠️ 文件 {csv_path} 已被占用，请先关闭后再运行脚本。")
        return False
    
    try:
        with open(json_path, "r", encoding="utf-8") as f:
            data = json.load(f)
    except Exception as e:
        print(f"❌ 读取 JSON 文件失败: {e}")
        return False
    
    rows = []
    for metric in data.get("Metrics", []):
        for metric_name, content in metric.items():
            logheader_template = content.get("logheader")
            colors = content.get("color", [])
            suffixes = content.get("suffix", [""])
            eyebox_ids = content.get("eyeboxId", [])

            if logheader_template:
                for color in colors:
                    for suffix in suffixes:
                        for eyebox in eyebox_ids:
                            expanded = (
                                logheader_template
                                .replace("$color$", color)
                                .replace("$suffix$", suffix)
                                .replace("$eyeboxId$", eyebox)
                            )
                            for logheader in expanded.split(","):
                                rows.append({
                                    "metric": metric_name,
                                    "logheader": logheader.strip(),
                                    "uplimit": -1,
                                    "lowlimit": -1
                                })

    try:
        df = pd.DataFrame(rows)
        df.to_csv(csv_path, index=False, encoding="utf-8-sig")
        print(f"✅ 已生成 CSV 文件：{csv_path}")
        print("请修改 CSV 文件中的 uplimit & lowlimit, 然后保存后继续执行。")
        return True
    except Exception as e:
        print(f"❌ 写入 CSV 失败: {e}")
        return False

def csv_to_json(csv_path: Path, json_path: Path):
    df = pd.read_csv(csv_path)

    output_dict = {
        row["logheader"]: {
            "uplimit": normalize_number(row["uplimit"]),
            "lowlimit": normalize_number(row["lowlimit"])
        }
        for _, row in df.iterrows()
    }

    with open(json_path, "w", encoding="utf-8") as f:
        json.dump(output_dict, f, ensure_ascii=False, indent=4)

    print(f"✅ 已生成 JSON 文件：{json_path}")

if __name__ == "__main__":
    cwd = Path.cwd()

    json_input = r"D:\CMakeProject\project\JiAn\src\RealityQ+\config\IQMetricConfig.json"
    csv_output = cwd / "logheader_limits.csv"
    json_output = cwd / "limits.json"

    if expand_logheaders(json_input, csv_output):
        input("修改完成 CSV 后按回车键继续...")
        csv_to_json(csv_output, json_output)
    else:
        print("⚠️ logheaders 输出到csv执行失败，程序终止。")
