# -*- coding: utf-8 -*-
from fastapi import FastAPI, APIRouter, UploadFile, File
from fastapi.middleware.cors import CORSMiddleware
from typing import List
from fastapi.responses import JSONResponse
import numpy as np
import cv2
import hyperlpr3 as lpr3
import uvicorn
import click

type_list = ["蓝牌", "黄牌单层", "白牌单层", "绿牌新能源", "黑牌港澳", "香港单层", "香港双层", "澳门单层", "澳门双层", "黄牌双层"]

catcher = lpr3.LicensePlateCatcher(detect_level=lpr3.DETECT_LEVEL_HIGH)

class BaseResponse():
    def __init__(self, *args, **kwags) -> None:
        self.response = {
            'result': None,
        }
        # self.response.update(response)

    def http_ok_response(self, response_data):
        self.response['result'] = response_data
        self.response['code'] = 5000
        self.response['msg'] = '请求成功'
        return JSONResponse(self.response)

    def http_prermission_denied_response(self, response_data=None, error_msg=None):
        """
        权限错误
        """
        self.response['result'] = response_data
        self.response['code'] = 5005
        if error_msg:
            self.response['msg'] = error_msg
        else:
            self.response['msg'] = '权限校验失败，请重新检查权限'

        return JSONResponse(self.response)

    def http_request_parameter_error(self, response_data=None, error_msg=None):
        """
        请求参数错误
        """
        self.response['result'] = response_data
        self.response['code'] = 5007
        if error_msg:
            self.response['msg'] = error_msg
        else:
            self.response['msg'] = '提交参数异常，请重新检查接口参数'

        return JSONResponse(self.response)

    def http_server_error(self, response_data=None, error_msg=None):
        self.response['result'] = response_data
        self.response['code'] = 5009
        if error_msg:
            self.response['msg'] = error_msg
        else:
            self.response['msg'] = '服务异常'

        return JSONResponse(self.response)


app = FastAPI(
    title="HyperLPR3-Api",
    version='0.0.9',
    docs_url='/api/v1/docs',
    description='HyperLPR3 Api Serving'
)

origins = [
    'http://localhost.slsmart.com',
    'https://localhost.slsmart.com',
    'http://localhost',
    'http://localhost:8715'
]

app.add_middleware(
    CORSMiddleware,
    allow_origins=origins,
    allow_credentials=True,
    allow_methods=['*'],
    allow_headers=['*'],
)


@app.get("/")
async def running():
    '''当前api服务程序有在正常运行'''
    return """HyperLpr3 WebApi Server Running..."""


@app.post("/api/v1/rec", tags=['车牌识别'])
async def vehicle_license_plate_recognition(file: List[UploadFile] = File(...)):
    """上传图片进行车牌识别，上传必须为图片类型png/jpg/jpge/wabp"""
    if len(file[0].filename) == 0:
        return BaseResponse().http_request_parameter_error(error_msg='单次上传图片不能为空')
    if len(file) > 1:
        return BaseResponse().http_request_parameter_error(error_msg='该接口仅支持单张图片上传')
    if len(file) == 1:
        if file[0].filename.rsplit('.', 1)[1].lower() not in ['png', 'jpeg', 'jpg', 'wabp']:
            return BaseResponse().http_request_parameter_error(error_msg='上传必须为图片类型png/jpg/jpge')
        content = await file[0].read()
        nparr = np.fromstring(content, np.uint8)
        img = cv2.imdecode(nparr, cv2.IMREAD_COLOR).astype(np.uint8)
        plates = catcher(img)
        results = list()
        for code, conf, plate_type, box in plates:
            if "nan" != f"{conf}":  # conf=nan会导致Json序列化错误
                plate = dict(code=code, conf=float(conf), plate_type=type_list[plate_type], box=box)
                results.append(plate)
        return BaseResponse().http_ok_response({'plate_list': results})


def get_application():
    return app


@click.command(help="Exec HyperLPR3 WebApi Server.")
@click.option("-host", "--host", default="0.0.0.0", type=str, )
@click.option("-port", "--port", default=8715, type=int, )
@click.option("-workers", "--workers", default=1, type=int, )
def rest(host, port, workers):
    uvicorn.run(app="hyperlpr3.command.serve:app", host=host, port=port, workers=workers)


if __name__ == "__main__":
    rest()
