import numpy as np
import tensorflow as tf
import matplotlib.pyplot as plt


def read_from_file(file, num_bytes, format_data=lambda x: int.from_bytes(x, byteorder='little')):
    data = []
    with open(file, 'rb') as f:
        block = bytes(f.read(num_bytes))
        while block:
            data.append(format_data(block))
            block = bytes(f.read(num_bytes))
    return data


def bits(num):
    bits_array = []
    for _ in range(0, 8):
        bits_array.append(num % 2)
        num //= 2
    return list(reversed(bits_array))


def _int(bit_array):
    value = 0
    for i in bit_array:
        value *= 2
        value += i
    return int(value)


def split_by_bit(block, bit=32):
    rv = []
    for i in block:
        rv += bits(i)
    return rv[:bit]


def split_by_byte(block, bytes=4):
    rv = []
    for i in block:
        rv.append(int(i))
    return rv[:bytes]


def to_float(block, bytes=4):
    return [int.from_bytes(block[:4], byteorder='little') / pow(2, 8 * bytes)]


def get_test_values(size=[32, 32]):
    __in = read_from_file("GOST_generator/out_x.bin", 8,
                          lambda x: split_by_bit(x, size[0]))
    __out = read_from_file("GOST_generator/out_y.bin", 8,
                           lambda x: split_by_bit(x, size[1]))

    return np.array(__in), np.array(__out)


# Deprecated version
def __multilayer_perceptron(x, weights, biases, num, activaion_fun=tf.nn.sigmoid):
    """
    Function to create layers.
    :param x: input data
    :param weights: vector of weights
    :param biases:  vector of biases
    :param num: number of layers(>=2)
    :return: output layer
    """

    # Hidden layer with activation
    layer_1 = tf.add(tf.matmul(x, weights['h1']), biases['b1'])
    layer_1 = activaion_fun(layer_1)
    layer = layer_1
    for i in range(2, num + 1):
        # Hidden layer with activation
        layer_new = tf.add(tf.matmul(layer, weights['h%d' % i]), biases['b%d' % i])
        layer_new = activaion_fun(layer_new)
        layer = layer_new

    # Output layer with linear activation
    out_layer = tf.matmul(layer, weights['out']) + biases['out']
    return out_layer


def __generate_weigths(n_input, n_hidden, n_classes, num):
    weights = {}
    for i in range(1, num + 1):
        weights['h%d' % i] = tf.Variable(tf.random.normal([n_input if i == 1 else n_hidden[i - 2], n_hidden[i-1]]))
    weights['out'] = tf.Variable(tf.random.normal([n_hidden[-1], n_classes]))
    return weights


def __generate_biases(n_input, n_hidden, n_classes, num):
    biases = {}
    for i in range(1, num + 1):
        biases['b%d' % i] = tf.Variable(tf.random.normal([n_hidden[i - 1]]))
        biases['out'] = tf.Variable(tf.random.normal([n_classes]))
    return biases


def multilayer_perceptron(x, hidden, num_classes, number_layers, activation_fun=tf.nn.sigmoid,
                          first_activation=False):
    """
    Function to create layers.
    :param x: input data
    :param hidden: vector of hidden layer's sizes
    :param num_classes:  size of output layer
    :param num: number of layers(>=2)
    :param activation_fun: activation function
    :param first_activation: use or not activation function on first hidden layer
    :return: output layer
    """

    # Hidden layer with activation
    layer = tf.layers.dense(x, hidden[0],
                            activation=activation_fun if first_activation else None,
                            kernel_initializer=tf.initializers.ones(),
                            bias_initializer=tf.initializers.ones())

    # Hidden layer with activation
    for i in range(1, number_layers):
        layer_new = tf.layers.dense(layer, hidden[i],
                                    activation=activation_fun,
                                    kernel_initializer=tf.initializers.ones(),
                                    bias_initializer=tf.initializers.ones())
        layer = layer_new

    # Output layer with linear activation
    out_layer = tf.layers.dense(layer, num_classes, activation=None)
    return out_layer


def init_network(input_data, output_data, n_input, n_hidden, n_classes, number_layers, learning_rate=0.001,
                 training_epochs=100, display_step=10, activation=tf.nn.sigmoid, optimizer=tf.train.AdamOptimizer):
    with open("log1.txt", "a+") as f:
        f.write('Start of neuron number_layers - {}\n'.format(number_layers))
    x = tf.placeholder(tf.float32, [None, n_input])
    y = tf.placeholder(tf.float32, [None, n_classes])

    # n_hidden = [n_hidden * i for i in range(1, number_layers + 1)]
    # n_hidden = [n_hidden] * number_layers
    # n_hidden = [int(sum([binomial(n_input, i)
    # for i in range(1, level + 1)])) for level in range(1, number_layers + 1)]
    # n_hidden = [sum([int(binomial(n_input, level))
    # for level in range(1, number_layers + 1)])] + [n_hidden] * (number_layers - 1)

    #prediction = multilayer_perceptron(x, n_hidden, n_classes, number_layers,
    #                                   activation_fun=activation, first_activation=True)

    prediction = __multilayer_perceptron(x,
                                         __generate_weigths(n_input, n_hidden, n_classes, number_layers),
                                         __generate_biases(n_input, n_hidden, n_classes, number_layers),
                                         number_layers)

    hamming_distance = tf.math.count_nonzero(tf.round(prediction) - y, axis=-1)
    accuracy = tf.reduce_mean(hamming_distance)
    cost = tf.reduce_mean(tf.square(prediction - y))

    loss = tf.losses.sigmoid_cross_entropy(y, prediction)
    loss1 = tf.keras.losses.binary_crossentropy(y, prediction)

    loss = cost

    optimizer = optimizer(learning_rate=learning_rate).minimize(loss)

    init = tf.global_variables_initializer()

    with tf.Session() as sess:
        sess.run(init)

        train_dataset = np.array(input_data[:9 * len(input_data) // 10])
        train_values = np.array(output_data[: 9 * len(input_data) // 10])

        test_dataset = np.array(input_data[-len(input_data) // 10:])
        test_values = np.array(output_data[-len(input_data) // 10:])

        for i in range(training_epochs):
            if i % display_step == 0:
                feed = {x: train_dataset, y: train_values}
                result = sess.run([loss, accuracy], feed_dict=feed)
                with open("log1.txt", "a+") as f:
                    f.write('Accuracy at step %s: %f - loss: %f\n' % (i, result[1], result[0]))
                print('Accuracy at step %s: %f - loss: %f\n' % (i, result[1], result[0]))
            else:
                feed = {x: train_dataset, y: train_values}
            sess.run(optimizer, feed_dict=feed)

        print("Optimization Finished!")

        test_values1 = sess.run(prediction, feed_dict={
            x: test_dataset,
        })

        # print(list(map(lambda x: hex(_int(x)), test_values1)))
        # print(list(map(lambda x: hex(_int(x)), test_values)))
        # print(list(map(lambda x: hex(_int(x)), test_dataset)))

        accuracy = tf.reduce_mean(tf.cast(hamming_distance, "float"))
        acc = accuracy.eval(feed_dict={x: test_dataset, y: test_values})
        los = loss.eval(feed_dict={x: test_dataset, y: test_values})

        with open("log1.txt", "a+") as f:
            f.write("testing accuracy: {}\n".format(acc))
            f.write("testing Loss: {}\n".format(los))
            f.write("\n\n\n\n")

        print("testing accuracy: {}".format(acc))
        print("testing Loss: {}".format(los))

        sess.close()

    return acc, los


if __name__ == "__main__":
    input_data, output_data = get_test_values([64, 64])

    # Network Parameters
    n_input = 64
    n_hidden = 64
    n_classes = 64
    number_layers = 2

    n_hidden = [64, 256]

    x, y = init_network(input_data, output_data, n_input, [64, 256], n_classes, 2,
                        activation=tf.nn.sigmoid, training_epochs=150000, display_step=1000)
                        
    x, y = init_network(input_data, output_data, n_input, [64, 128], n_classes, 2,
                        activation=tf.nn.sigmoid, training_epochs=150000, display_step=1000)                    
                        
    x, y = init_network(input_data, output_data, n_input, [64, 128, 256], n_classes, 3,
                        activation=tf.nn.sigmoid, training_epochs=150000, display_step=1000)
    
    x, y = init_network(input_data, output_data, n_input, [64, 256, 512], n_classes, 3,
                        activation=tf.nn.sigmoid, training_epochs=150000, display_step=1000)

