import requests
from tqdm import tqdm
import zipfile
import tarfile
import os
import sys
from urllib.parse import urljoin, urlparse
from .settings import _DEFAULT_FOLDER_, _MODEL_VERSION_, _ONLINE_URL_, _REMOTE_URL_, onnx_model_maps, onnx_runtime_config


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
    name = os.path.normpath(name)  # Ensure the path is correct for the platform

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
        if name.endswith('.zip'):
            with zipfile.ZipFile(name, "r") as f:
                f.extractall(save_path)
        elif name.endswith(('.tar.gz', '.tgz')):
            with tarfile.open(name, "r:gz") as f:
                f.extractall(save_path)
        else:
            raise ValueError("Unsupported archive format: " + name)
        os.remove(name)


def initialization(re_download=False):
    os.makedirs(_DEFAULT_FOLDER_, exist_ok=True)
    models_dir = os.path.join(_DEFAULT_FOLDER_, _MODEL_VERSION_)
    models_dir = os.path.normpath(models_dir)  # Ensure the path is correct for the platform

    if not os.path.exists(models_dir) or re_download:
        target_url = urljoin(_ONLINE_URL_, _MODEL_VERSION_ + '.zip')
        if not urlparse(target_url).scheme:
            raise ValueError(f"Invalid URL scheme in {target_url}")
        down_model_zip(target_url, _DEFAULT_FOLDER_, True)
