"""
Author: youngorsu
Email : zhiyongsu@qq.com
Last edited: 2018.1.29
"""
# coding=utf-8


import sys
import os
from PyQt5.QtWidgets import (
    QMainWindow,
    QLabel,
    QLineEdit,
    QPushButton,
    QHBoxLayout,
    QVBoxLayout,
    QGridLayout,
    QTableWidget,
    QWidget,
    QAbstractItemView,
    QHeaderView,
    QGraphicsView,
    QGraphicsScene,
    QGraphicsPixmapItem,
    QSplitter,
    QFileDialog,
    QTableWidgetItem,
    QGraphicsRectItem,
    QCheckBox,
    QMessageBox,
    QGroupBox,
    QGraphicsSimpleTextItem,
    qApp,
    QAction,
    QApplication)
from PyQt5.QtGui import QIcon, QColor, QPainter, QImage, QPixmap, QPen, QBrush, QFont, QPalette, QKeySequence
from PyQt5.QtCore import Qt, QDir, QSize, QEventLoop, QThread, pyqtSignal

from hyperlpr_py3 import pipline as pp

import cv2

import numpy as np

import time

import shutil

draw_plate_in_image_enable = 1

plateTypeName = ["蓝", "黄", "绿", "白", "黑 "]


def SimpleRecognizePlateWithGui(image):
    t0 = time.time()

    images = pp.detect.detectPlateRough(
        image, image.shape[0], top_bottom_padding_rate=0.1)

    res_set = []
    y_offset = 32
    for j, plate in enumerate(images):
        plate, rect, origin_plate = plate

        plate = cv2.resize(plate, (136, 36 * 2))
        t1 = time.time()

        plate_type = pp.td.SimplePredict(plate)
        plate_color = plateTypeName[plate_type]

        if (plate_type > 0) and (plate_type < 5):
            plate = cv2.bitwise_not(plate)

        if draw_plate_in_image_enable == 1:
            image[y_offset:y_offset + plate.shape[0], 0:plate.shape[1]] = plate
            y_offset = y_offset + plate.shape[0] + 4

        image_rgb = pp.fm.findContoursAndDrawBoundingBox(plate)

        if draw_plate_in_image_enable == 1:
            image[y_offset:y_offset + image_rgb.shape[0],
                  0:image_rgb.shape[1]] = image_rgb
            y_offset = y_offset + image_rgb.shape[0] + 4

        image_rgb = pp.fv.finemappingVertical(image_rgb)

        if draw_plate_in_image_enable == 1:
            image[y_offset:y_offset + image_rgb.shape[0],
                  0:image_rgb.shape[1]] = image_rgb
            y_offset = y_offset + image_rgb.shape[0] + 4

        pp.cache.verticalMappingToFolder(image_rgb)

        if draw_plate_in_image_enable == 1:
            image[y_offset:y_offset + image_rgb.shape[0],
                  0:image_rgb.shape[1]] = image_rgb
            y_offset = y_offset + image_rgb.shape[0] + 4

        e2e_plate, e2e_confidence = pp.e2e.recognizeOne(image_rgb)
        print("e2e:", e2e_plate, e2e_confidence)

        image_gray = cv2.cvtColor(image_rgb, cv2.COLOR_RGB2GRAY)

        #print("校正", time.time() - t1, "s")

        t2 = time.time()
        val = pp.segmentation.slidingWindowsEval(image_gray)
        # print val
        #print("分割和识别", time.time() - t2, "s")

        res=""
        confidence = 0
        if len(val) == 3:
            blocks, res, confidence = val
            if confidence / 7 > 0.7:

                if draw_plate_in_image_enable == 1:
                    image = pp.drawRectBox(image, rect, res)
                    for i, block in enumerate(blocks):
                        block_ = cv2.resize(block, (24, 24))
                        block_ = cv2.cvtColor(block_, cv2.COLOR_GRAY2BGR)
                        image[j * 24:(j * 24) + 24, i *
                              24:(i * 24) + 24] = block_
                        if image[j * 24:(j * 24) + 24,
                                 i * 24:(i * 24) + 24].shape == block_.shape:
                            pass

        res_set.append([res,
                        confidence / 7,
                        rect,
                        plate_color,
                        e2e_plate,
                        e2e_confidence,
                        len(blocks)])
        print("seg:",res,confidence/7)
    #print(time.time() - t0, "s")

    print("---------------------------------")
    return image, res_set


class LicenseRecognizationThread(QThread):

    recognization_done_signal = pyqtSignal(list)

    def __init__(self, parent=None):
        super().__init__(parent)
        self.hyperlpr_dir_path = ""
        self.filenames = []

    def set_parameter(self, filename_list, path):
        self.hyperlpr_dir_path = path
        self.filenames = filename_list

    def run(self):
        while True:
            time.sleep(1)
            if len(self.hyperlpr_dir_path) > 0:
                for i in range(0, len(self.filenames)):
                    path = os.path.join(
                        self.hyperlpr_dir_path, self.filenames[i])
                    image = cv2.imdecode(np.fromfile(path, dtype=np.uint8), -1)
                    image, res_set = SimpleRecognizePlateWithGui(image)
                    self.recognization_done_signal.emit([i, res_set])

                self.hyperlpr_dir_path = ""


class HyperLprImageView(QGraphicsView):

    def __init__(self):

        super().__init__()

        self.init_ui()

    def init_ui(self):

        scene = QGraphicsScene()
        scene.setBackgroundBrush(QColor(100, 100, 100))
        scene.setItemIndexMethod(QGraphicsScene.BspTreeIndex)

        scene.setSceneRect(scene.itemsBoundingRect())

        self.setDragMode(QGraphicsView.RubberBandDrag)
        self.setViewportUpdateMode(QGraphicsView.FullViewportUpdate)
        self.setRenderHints(QPainter.Antialiasing | QPainter.TextAntialiasing)

        self.frame_item = QGraphicsPixmapItem()

        self.text_item_offset = 0
        self.rect_item_array = []
        self.text_item_array = []
        for i in range(0, 5):
            rect_item = QGraphicsRectItem()
            rect_item.setVisible(False)
            rect_item.setZValue(20.0)
            rect_item.setPen(QPen(Qt.red, 5))
            rect_item.setRect(20, 20, 20, 20)
            scene.addItem(rect_item)
            self.rect_item_array.append(rect_item)
            text_item = QGraphicsSimpleTextItem("")
            text_item.setBrush(QBrush(Qt.red))
            text_item.setZValue(20.0)
            text_item.setPos(10, 50)
            text_item.setFont(QFont("黑体", 24))
            text_item.setVisible(False)
            scene.addItem(text_item)
            self.text_item_array.append(text_item)

        scene.addItem(self.frame_item)

        self.curr_factor = 1.0

        self.setScene(scene)

    def resetRectText(self, res_set):
        max_no = len(res_set)

        if max_no > 5:
            max_no = 5

        for i in range(0, 5):
            if i < max_no:
                curr_rect = res_set[i][2]
                self.rect_item_array[i].setRect(int(curr_rect[0]), int(
                    curr_rect[1]), int(curr_rect[2]), int(curr_rect[3]))
                self.rect_item_array[i].setVisible(True)

                self.text_item_array[i].setText(
                    res_set[i][4] + " " + res_set[i][3])
                self.text_item_array[i].setPos(
                    int(curr_rect[0]), int(curr_rect[1]) - 48)
                self.text_item_array[i].setVisible(True)
            else:
                self.text_item_array[i].setVisible(False)
                self.rect_item_array[i].setVisible(False)

    def wheelEvent(self, event):
        factor = event.angleDelta().y() / 120.0
        if event.angleDelta().y() / 120.0 > 0:
            factor = 1.08
        else:
            factor = 0.92

        if self.curr_factor > 0.1 and self.curr_factor < 10:
            self.curr_factor = self.curr_factor * factor
            self.scale(factor, factor)

    def resetPixmap(self, image):

        self.frame_item.setPixmap(QPixmap.fromImage(image))


class HyperLprWindow(QMainWindow):

    start_init_signal = pyqtSignal()

    def __init__(self):

        super().__init__()

        self.initUI()

    def initUI(self):

        self.statusBar().showMessage('Ready')

        self.left_action = QAction('上一个', self)
        self.left_action.setShortcut(QKeySequence.MoveToPreviousChar)
        self.left_action.triggered.connect(self.analyze_last_one_image)

        self.right_action = QAction('下一个', self)
        self.right_action.setShortcut(QKeySequence.MoveToNextChar)
        self.right_action.triggered.connect(self.analyze_next_one_image)

        self.rename_image_action = QAction('保存e2e文件名', self)
        self.rename_image_action.setShortcut(QKeySequence.MoveToPreviousLine)
        self.rename_image_action.triggered.connect(self.rename_current_image_with_info)

        self.statusBar()

        menubar = self.menuBar()
        fileMenu = menubar.addMenu('&Function')
        fileMenu.addAction(self.left_action)
        fileMenu.addAction(self.right_action)
        fileMenu.addAction(self.rename_image_action)

        self.image_window_view = HyperLprImageView()

        table_widget_header_labels = [
            "文件名",
            "分割识别",
            "置信度",
            "颜色",
            "E2E识别",
            "E2E置信度"]

        self.hyperlpr_tableview = QTableWidget(
            0, len(table_widget_header_labels))
        self.hyperlpr_tableview.setHorizontalHeaderLabels(
            table_widget_header_labels)

        self.hyperlpr_tableview.setSelectionBehavior(
            QAbstractItemView.SelectItems)
        self.hyperlpr_tableview.setSelectionMode(
            QAbstractItemView.SingleSelection)
        self.hyperlpr_tableview.setEditTriggers(
            QAbstractItemView.NoEditTriggers)
        self.hyperlpr_tableview.horizontalHeader().setSectionResizeMode(
            QHeaderView.ResizeToContents)
        self.hyperlpr_tableview.setEditTriggers(
            QAbstractItemView.NoEditTriggers)

        self.hyperlpr_tableview.cellClicked.connect(
            self.recognize_one_license_plate)

        self.left_button = QPushButton("<")
        self.left_button.setFixedWidth(60)
        self.right_button = QPushButton(">")
        self.right_button.setFixedWidth(60)
        self.left_button.setEnabled(False)
        self.right_button.setEnabled(False)
        self.left_button.clicked.connect(self.analyze_last_one_image)
        self.right_button.clicked.connect(self.analyze_next_one_image)
        left_right_layout = QHBoxLayout()
        left_right_layout.addStretch()
        left_right_layout.addWidget(self.left_button)
        left_right_layout.addStretch()
        left_right_layout.addWidget(self.right_button)
        left_right_layout.addStretch()

        self.location_label = QLabel("车牌目录", self)
        self.location_text = QLineEdit(self)
        self.location_text.setEnabled(False)
        #self.location_text.setFixedWidth(300)
        self.location_button = QPushButton("...")
        self.location_button.clicked.connect(self.select_new_dir)

        self.location_layout = QHBoxLayout()
        self.location_layout.addWidget(self.location_label)
        self.location_layout.addWidget(self.location_text)
        self.location_layout.addWidget(self.location_button)
        self.location_layout.addStretch()

        self.check_box = QCheckBox("与文件名比较车牌")
        self.check_box.setChecked(True)

        self.update_file_path_button = QPushButton('批量识别')
        self.update_file_path_button.clicked.connect(
            self.batch_recognize_all_images)

        self.update_file_path_layout = QHBoxLayout()
        self.update_file_path_layout.addWidget(self.check_box)
        self.update_file_path_layout.addWidget(self.update_file_path_button)
        self.update_file_path_layout.addStretch()

        self.save_as_e2e_filename_button = QPushButton("保存e2e文件名")
        self.save_as_e2e_filename_button.setEnabled(False)
        self.save_as_e2e_filename_button.clicked.connect(self.rename_current_image_with_info)
        self.save_layout = QHBoxLayout()
        self.save_layout.addWidget(self.save_as_e2e_filename_button)
        self.save_layout.addStretch()

        self.top_layout = QVBoxLayout()
        self.top_layout.addLayout(left_right_layout)
        self.top_layout.addLayout(self.location_layout)
        self.top_layout.addLayout(self.update_file_path_layout)
        self.top_layout.addLayout(self.save_layout)

        function_groupbox = QGroupBox("功能区")
        function_groupbox.setLayout(self.top_layout)

        license_plate_image_label = QLabel("车牌图")
        self.license_plate_widget = QLabel("")

        block_image_label = QLabel("分割图")
        self.block_plate_widget = QLabel("")

        filename_label = QLabel("文件名：")
        self.filename_edit = QLineEdit()

        segmentation_recognition_label = QLabel("分割识别：")
        self.segmentation_recognition_edit = QLineEdit()
        self.segmentation_recognition_edit.setFont(QFont("黑体", 24, QFont.Bold))
        # self.segmentation_recognition_edit.setStyleSheet("color:red")

        confidence_label = QLabel("分割识别\n置信度")
        self.confidence_edit = QLineEdit()
        #self.confidence_edit.setFont(QFont("黑体", 24, QFont.Bold))
        # self.confidence_edit.setStyleSheet("color:red")

        plate_color_label = QLabel("车牌颜色")
        self.plate_color_edit = QLineEdit()
        self.plate_color_edit.setFont(QFont("黑体", 24, QFont.Bold))
        # self.plate_color_edit.setStyleSheet("color:red")

        e2e_recognization_label = QLabel("e2e识别：")
        self.e2e_recognization_edit = QLineEdit()
        self.e2e_recognization_edit.setFont(QFont("黑体", 24, QFont.Bold))
        # self.e2e_recognization_edit.setStyleSheet("color:red")

        e2e_confidence_label = QLabel("e2e置信度")
        self.e2e_confidence_edit = QLineEdit()
        #self.e2e_confidence_edit.setFont(QFont("黑体", 24, QFont.Bold))
        # self.e2e_confidence_edit.setStyleSheet("color:red")

        info_gridlayout = QGridLayout()
        line_index = 0
        info_gridlayout.addWidget(filename_label, line_index, 0)
        info_gridlayout.addWidget(self.filename_edit, line_index, 1)
        line_index += 1
        info_gridlayout.addWidget(license_plate_image_label, line_index, 0)
        info_gridlayout.addWidget(self.license_plate_widget, line_index, 1)
        line_index += 1
        info_gridlayout.addWidget(e2e_recognization_label, line_index, 0)
        info_gridlayout.addWidget(self.e2e_recognization_edit, line_index, 1)
        line_index += 1
        info_gridlayout.addWidget(
            segmentation_recognition_label, line_index, 0)
        info_gridlayout.addWidget(
            self.segmentation_recognition_edit, line_index, 1)
        line_index += 1
        info_gridlayout.addWidget(plate_color_label, line_index, 0)
        info_gridlayout.addWidget(self.plate_color_edit, line_index, 1)
        line_index += 1
        info_gridlayout.addWidget(block_image_label, line_index, 0)
        info_gridlayout.addWidget(self.block_plate_widget, line_index, 1)
        line_index += 1
        info_gridlayout.addWidget(confidence_label, line_index, 0)
        info_gridlayout.addWidget(self.confidence_edit, line_index, 1)
        line_index += 1
        info_gridlayout.addWidget(e2e_confidence_label, line_index, 0)
        info_gridlayout.addWidget(self.e2e_confidence_edit, line_index, 1)

        info_widget = QGroupBox("分割识别&e2e")

        info_widget.setLayout(info_gridlayout)

        right_splitter = QSplitter(Qt.Vertical)
        right_splitter.addWidget(self.hyperlpr_tableview)
        right_splitter.addWidget(function_groupbox)
        right_splitter.addWidget(info_widget)
        right_splitter.setStretchFactor(0, 2)
        right_splitter.setStretchFactor(2, 1)

        main_splitter = QSplitter(Qt.Horizontal)
        main_splitter.addWidget(self.image_window_view)
        main_splitter.addWidget(right_splitter)
        main_splitter.setStretchFactor(0, 1)

        self.image_filename_list = []
        self.hyperlpr_dir_path = ""
        self.segmentation_recognition_correct_number = 0
        self.color_correct_number = 0
        self.e2e_recognization_correct_number = 0
        self.current_row = 0

        self.batch_recognization_thread = LicenseRecognizationThread()
        self.batch_recognization_thread.recognization_done_signal.connect(
            self.recognization_done_slot)
        self.batch_recognization_thread.start()

        self.start_init_signal.connect(self.read_path_and_show_one_image)

        self.setCentralWidget(main_splitter)

        self.setWindowTitle("HyperLPR车牌识别软件v1.0")

        self.start_init_signal.emit()

    def read_path_and_show_one_image(self):

        hyperlpr_dir_info_filepath = QDir.homePath() + "/hyperlpr_dir_file"
        if os.path.exists(hyperlpr_dir_info_filepath):
            with open(hyperlpr_dir_info_filepath, 'r') as f:
                self.hyperlpr_dir_path = f.read()

        if len(self.hyperlpr_dir_path) > 0:
            self.reset_info_gui()

        if len(self.image_filename_list) > 0:
            self.recognize_and_show_one_image(self.image_filename_list[0], 0)

    def select_new_dir(self):

        self.hyperlpr_dir_path = QFileDialog.getExistingDirectory(
            self, "读取文件夹", QDir.currentPath())

        if len(self.hyperlpr_dir_path) > 0:
            hyperlpr_dir_info_filepath = QDir.homePath() + "/hyperlpr_dir_file"
            with open(hyperlpr_dir_info_filepath, 'w') as f:
                f.write(self.hyperlpr_dir_path)
            self.reset_info_gui()

    def rename_current_image_with_info(self):
        if len(self.hyperlpr_dir_path) > 0:
            target_dir_path = self.hyperlpr_dir_path + "/result"
            if not os.path.exists(target_dir_path):
                os.makedirs(target_dir_path)
            if len(self.plate_color_edit.text())>0 and len(self.e2e_recognization_edit.text())>0:
                orign_path = os.path.join(self.hyperlpr_dir_path, self.filename_edit.text())
                target_path = os.path.join(target_dir_path,self.plate_color_edit.text()+"-"+self.e2e_recognization_edit.text()+".jpg")
                shutil.copyfile(orign_path, target_path)

    def reset_info_gui(self):

        self.location_text.setText(self.hyperlpr_dir_path)
        self.scan_files_with_new_dir(self.hyperlpr_dir_path)
        self.fill_table_with_new_info()

    def scan_files_with_new_dir(self, path):

        name_list = os.listdir(path)  # 列出文件夹下所有的目录与文件
        self.image_filename_list.clear()
        for i in range(0, len(name_list)):
            if name_list[i].endswith(
                    ".jpg") or name_list[i].endswith(".png"):
                self.image_filename_list.append(name_list[i])

    def fill_table_with_new_info(self):
        self.hyperlpr_tableview.clearContents()
        row_count = self.hyperlpr_tableview.rowCount()
        for i in range(row_count, -1, -1):
            self.hyperlpr_tableview.removeRow(i)

        for i in range(0, len(self.image_filename_list)):
            row = self.hyperlpr_tableview.rowCount()
            self.hyperlpr_tableview.insertRow(row)

            item0 = QTableWidgetItem()
            item0.setTextAlignment(Qt.AlignCenter)
            self.hyperlpr_tableview.setItem(row, 0, item0)
            self.hyperlpr_tableview.item(
                row, 0).setText(
                self.image_filename_list[i])

            item1 = QTableWidgetItem()
            item1.setTextAlignment(Qt.AlignCenter)
            self.hyperlpr_tableview.setItem(row, 1, item1)

            item2 = QTableWidgetItem()
            item2.setTextAlignment(Qt.AlignCenter)
            self.hyperlpr_tableview.setItem(row, 2, item2)

            item3 = QTableWidgetItem()
            item3.setTextAlignment(Qt.AlignCenter)
            self.hyperlpr_tableview.setItem(row, 3, item3)

            item4 = QTableWidgetItem()
            item4.setTextAlignment(Qt.AlignCenter)
            self.hyperlpr_tableview.setItem(row, 4, item4)

            item5 = QTableWidgetItem()
            item5.setTextAlignment(Qt.AlignCenter)
            self.hyperlpr_tableview.setItem(row, 5, item5)

        if len(self.image_filename_list) > 0:
            self.left_button.setEnabled(True)
            self.right_button.setEnabled(True)
            self.save_as_e2e_filename_button.setEnabled(True)

    def analyze_last_one_image(self):
        if self.current_row > 0:
            self.recognize_one_license_plate(self.current_row-1, 0)

    def analyze_next_one_image(self):
        if self.current_row < (len(self.image_filename_list)-1):
            self.recognize_one_license_plate(self.current_row + 1, 0)

    def recognize_one_license_plate(self, row, col):
        if col == 0 and row < len(self.image_filename_list):
            self.current_row = row
            self.recognize_and_show_one_image(
                self.image_filename_list[row], row)

    def recognize_and_show_one_image(self, image_filename_text, row):

        if image_filename_text.endswith(".jpg"):

            print(image_filename_text)
            path = os.path.join(self.hyperlpr_dir_path, image_filename_text)
            image = cv2.imdecode(np.fromfile(path, dtype=np.uint8), -1)
            image, res_set = SimpleRecognizePlateWithGui(image)
            img = QImage(
                image.data,
                image.shape[1],
                image.shape[0],
                image.shape[1] * image.shape[2],
                QImage.Format_RGB888)
            self.image_window_view.resetPixmap(img.rgbSwapped())
            self.image_window_view.resetRectText(res_set)

            if len(res_set) > 0:
                curr_rect = res_set[0][2]
                image_crop = image[int(curr_rect[1]):int(
                    curr_rect[1] + curr_rect[3]), int(curr_rect[0]):int(curr_rect[0] + curr_rect[2])]
                curr_plate = cv2.resize(image_crop, (204, 108))
                plate_img = QImage(
                    curr_plate.data,
                    curr_plate.shape[1],
                    curr_plate.shape[0],
                    curr_plate.shape[1] *
                    curr_plate.shape[2],
                    QImage.Format_RGB888)
                self.license_plate_widget.setPixmap(
                    QPixmap.fromImage(plate_img.rgbSwapped()))

                # print(res_set[0][6])
                block_crop = image[0:24, 0:(24 * int(res_set[0][6]))]
                curr_block = cv2.resize(
                    block_crop, (24 * int(res_set[0][6]), 24))
                block_image = QImage(
                    curr_block.data,
                    curr_block.shape[1],
                    curr_block.shape[0],
                    curr_block.shape[1] *
                    curr_block.shape[2],
                    QImage.Format_RGB888)
                self.block_plate_widget.setPixmap(
                    QPixmap.fromImage(block_image.rgbSwapped()))

                self.segmentation_recognition_edit.setText(res_set[0][0])
                if res_set[0][0] in image_filename_text:
                    self.segmentation_recognition_edit.setStyleSheet("color:black")
                else:
                    self.segmentation_recognition_edit.setStyleSheet("color:red")


                self.filename_edit.setText(image_filename_text)
                self.confidence_edit.setText("%.3f" % (float(res_set[0][1])))

                self.plate_color_edit.setText(res_set[0][3])
                if res_set[0][3] in image_filename_text:
                    self.plate_color_edit.setStyleSheet("color:black")
                else:
                    self.plate_color_edit.setStyleSheet("color:red")

                self.e2e_recognization_edit.setText(res_set[0][4])
                if res_set[0][4] in image_filename_text:
                    self.e2e_recognization_edit.setStyleSheet("color:black")
                else:
                    self.e2e_recognization_edit.setStyleSheet("color:red")

                self.e2e_confidence_edit.setText(
                    "%.3f" % (float(res_set[0][5])))
            else:
                self.license_plate_widget.clear()
                self.block_plate_widget.clear()
                self.segmentation_recognition_edit.setText("")
                self.filename_edit.setText(image_filename_text)
                self.confidence_edit.setText("")
                self.plate_color_edit.setText("")
                self.e2e_recognization_edit.setText("")
                self.e2e_confidence_edit.setText("")

            self.fill_table_widget_with_res_info(res_set, row)

    def batch_recognize_all_images(self):
        self.segmentation_recognition_correct_number = 0
        self.color_correct_number = 0
        self.e2e_recognization_correct_number = 0
        self.batch_recognization_thread.set_parameter(
            self.image_filename_list, self.hyperlpr_dir_path)

    def recognization_done_slot(self, result_list):
        row = result_list[0]
        res_set = result_list[1]
        self.fill_table_widget_with_res_info(res_set, row)

        if row == len(self.image_filename_list) - 1:
            total_number = len(self.image_filename_list)

            row_count = self.hyperlpr_tableview.rowCount()
            if row_count > total_number:
                self.hyperlpr_tableview.removeRow(total_number)

            self.hyperlpr_tableview.insertRow(total_number)

            item0 = QTableWidgetItem()
            item0.setTextAlignment(Qt.AlignCenter)
            self.hyperlpr_tableview.setItem(total_number, 0, item0)
            self.hyperlpr_tableview.item(
                total_number, 0).setText(
                "统计结果")

            item1 = QTableWidgetItem()
            item1.setTextAlignment(Qt.AlignCenter)
            self.hyperlpr_tableview.setItem(total_number, 1, item1)
            self.hyperlpr_tableview.item(
                total_number,
                1).setText(
                "{0} / {1} = {2: .3f}".format(
                    self.segmentation_recognition_correct_number,
                    total_number,
                    self.segmentation_recognition_correct_number /
                    total_number))

            item2 = QTableWidgetItem()
            item2.setTextAlignment(Qt.AlignCenter)
            self.hyperlpr_tableview.setItem(total_number, 2, item2)

            item3 = QTableWidgetItem()
            item3.setTextAlignment(Qt.AlignCenter)
            self.hyperlpr_tableview.setItem(total_number, 3, item3)
            self.hyperlpr_tableview.item(
                total_number, 3).setText(
                "{0} / {1} = {2: .3f}".format(self.e2e_recognization_correct_number, total_number,
                                              self.e2e_recognization_correct_number / total_number))

            item4 = QTableWidgetItem()
            item4.setTextAlignment(Qt.AlignCenter)
            self.hyperlpr_tableview.setItem(total_number, 4, item4)
            self.hyperlpr_tableview.item(
                total_number, 4).setText(
                "{0} / {1} = {2: .3f}".format(self.color_correct_number, total_number,
                                              self.color_correct_number / total_number))

            item5 = QTableWidgetItem()
            item5.setTextAlignment(Qt.AlignCenter)
            self.hyperlpr_tableview.setItem(total_number, 5, item5)

    def fill_table_widget_with_res_info(self, res_set, row):
        image_filename_text = self.image_filename_list[row]
        if len(res_set) > 0:

            self.hyperlpr_tableview.item(row, 1).setText(res_set[0][0])
            if res_set[0][0] in image_filename_text:
                self.hyperlpr_tableview.item(
                    row, 1).setForeground(
                    QBrush(
                        QColor(
                            0, 0, 255)))
                self.segmentation_recognition_correct_number += 1
            else:
                self.hyperlpr_tableview.item(
                    row, 1).setForeground(
                    QBrush(
                        QColor(
                            255, 0, 0)))

            self.hyperlpr_tableview.item(
                row, 2).setText(
                "%.3f" %
                (float(
                    res_set[0][1])))

            self.hyperlpr_tableview.item(row, 3).setText(res_set[0][3])
            if res_set[0][3] in image_filename_text:
                self.hyperlpr_tableview.item(
                    row, 3).setForeground(
                    QBrush(
                        QColor(
                            0, 0, 255)))
                self.color_correct_number += 1
            else:
                self.hyperlpr_tableview.item(
                    row, 3).setForeground(
                    QBrush(
                        QColor(
                            255, 0, 0)))

            self.hyperlpr_tableview.item(row, 4).setText(res_set[0][4])
            if res_set[0][4] in image_filename_text:
                self.hyperlpr_tableview.item(
                    row, 4).setForeground(
                    QBrush(
                        QColor(
                            0, 0, 255)))
                self.e2e_recognization_correct_number += 1
            else:
                self.hyperlpr_tableview.item(
                    row, 4).setForeground(
                    QBrush(
                        QColor(
                            255, 0, 0)))

            self.hyperlpr_tableview.item(
                row, 5).setText(
                "%.3f" %
                (float(
                    res_set[0][5])))


if __name__ == '__main__':

    app = QApplication(sys.argv)

    hyper_lpr_widow = HyperLprWindow()

    hyper_lpr_widow.showMaximized()

    sys.exit(app.exec_())
