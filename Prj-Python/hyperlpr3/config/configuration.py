import requests
from tqdm import tqdm
import zipfile
import os
from .settings import _DEFAULT_FOLDER_, _MODEL_VERSION_, _ONLINE_URL_, _REMOTE_URL_, onnx_model_maps, onnx_runtime_config


def safe_extract_zip(zip_file, save_path):
    root = os.path.realpath(save_path)
    for member in zip_file.infolist():
        target_path = os.path.realpath(os.path.join(save_path, member.filename))
        if target_path != root and not target_path.startswith(root + os.sep):
            raise RuntimeError(f"Blocked unsafe zip member path: {member.filename}")
        if (member.external_attr >> 16) & 0o170000 == 0o120000:
            raise RuntimeError(f"Blocked unsafe zip symlink: {member.filename}")

    zip_file.extractall(save_path)


def down_model_file(url, save_path):
    resp = requests.get(url, stream=True)
    total = int(resp.headers.get('content-length', 0))
    with open(save_path, 'wb') as file, tqdm(
            desc="Pull",
            total=total,
            unit='iB',
            unit_scale=True,
            unit_divisor=1024,
    ) as bar:
        for data in resp.iter_content(chunk_size=1024):
            size = file.write(data)
            bar.update(size)


def down_model_zip(url, save_path, is_unzip=False):
    resp = requests.get(url, stream=True)
    total = int(resp.headers.get('content-length', 0))
    name = os.path.join(save_path, os.path.basename(url))
    with open(name, 'wb') as file, tqdm(
            desc="Pull",
            total=total,
            unit='iB',
            unit_scale=True,
            unit_divisor=1024,
    ) as bar:
        for data in resp.iter_content(chunk_size=1024):
            size = file.write(data)
            bar.update(size)

    if is_unzip:
        with zipfile.ZipFile(name, "r") as f:
            safe_extract_zip(f, save_path)
        os.remove(name)


# def initialization(re_download=False):
#     models_dir = os.path.join(_DEFAULT_FOLDER_, _MODEL_VERSION_, "onnx")
#     os.makedirs(models_dir, exist_ok=True)
#     for model_key in onnx_model_maps:
#         save_path = onnx_runtime_config[model_key]
#         basename = os.path.basename(save_path)
#         remote_url = os.path.join(_REMOTE_URL_, basename + "?raw=true")
#         down_path = os.path.join(models_dir, basename)
#         if not os.path.exists(down_path) or re_download:
#             down_model_file(remote_url, down_path)

def initialization(re_download=False):
    os.makedirs(_DEFAULT_FOLDER_, exist_ok=True)
    models_dir = os.path.join(_DEFAULT_FOLDER_, _MODEL_VERSION_)
    # print(models_dir)
    if not os.path.exists(models_dir) or re_download:
        target_url = os.path.join(_ONLINE_URL_, _MODEL_VERSION_) + '.zip'
        down_model_zip(target_url, _DEFAULT_FOLDER_, True)
