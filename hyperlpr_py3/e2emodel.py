
from keras import backend as K
from keras.models import *
from keras.layers import *
from . import e2e


def ctc_lambda_func(args):
    y_pred, labels, input_length, label_length = args
    y_pred = y_pred[:, 2:, :]
    return K.ctc_batch_cost(labels, y_pred, input_length, label_length)


def construct_model(model_path):
    input_tensor = Input((None, 40, 3))
    x = input_tensor
    base_conv = 32

    for i in range(3):
        x = Conv2D(base_conv * (2 ** (i)), (3, 3),padding="same")(x)
        x = BatchNormalization()(x)
        x = Activation('relu')(x)
        x = MaxPooling2D(pool_size=(2, 2))(x)
    x = Conv2D(256, (5, 5))(x)
    x = BatchNormalization()(x)
    x = Activation('relu')(x)
    x = Conv2D(1024, (1, 1))(x)
    x = BatchNormalization()(x)
    x = Activation('relu')(x)
    x = Conv2D(len(e2e.chars)+1, (1, 1))(x)
    x = Activation('softmax')(x)
    base_model = Model(inputs=input_tensor, outputs=x)
    base_model.load_weights(model_path)
    return base_model
