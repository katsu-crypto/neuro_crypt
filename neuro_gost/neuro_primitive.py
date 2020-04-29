import matplotlib.pyplot as plt
import numpy as np

import networks
import utils

models = {
    "g1": ("GOST_generator/bin/g1_x.bin", "GOST_generator/bin/g1_y.bin",
           lambda x: utils.split_by_bit(x, 8), lambda x: utils.split_by_bit(x, 4), 1),
    "g2": ("GOST_generator/bin/g2_x.bin", "GOST_generator/bin/g2_y.bin",
           lambda x: utils.split_by_bit(x, 8), lambda x: utils.split_by_bit(x, 4), 1),
    "g0": ("GOST_generator/bin/g0_x.bin", "GOST_generator/bin/g0_y.bin",
           lambda x: utils.split_by_bit(x, 8), lambda x: utils.split_by_bit(x, 4), 1),
    "g3": ("GOST_generator/bin/g3_x.bin", "GOST_generator/bin/g3_y.bin",
           lambda x: utils.split_by_bit(x, 32), lambda x: utils.split_by_bit(x, 32), 4),
    "f0": ("GOST_generator/bin/f0_x.bin", "GOST_generator/bin/f0_y.bin",
           lambda x: utils.split_by_bit(x, 16), lambda x: utils.split_by_bit(x, 16), 2),
    "f1": ("GOST_generator/bin/f1_x.bin", "GOST_generator/bin/f1_y.bin",
           lambda x: utils.split_by_bit(x, 16), lambda x: utils.split_by_bit(x, 16), 2),
    "f2": ("GOST_generator/bin/f2_x.bin", "GOST_generator/bin/f2_y.bin",
           lambda x: utils.split_by_bit(x, 16), lambda x: utils.split_by_bit(x, 16), 2),
    "f3": ("GOST_generator/bin/f3_x.bin", "GOST_generator/bin/f3_y.bin",
           lambda x: utils.split_by_bit(x, 16), lambda x: utils.split_by_bit(x, 16), 2),
    "f4": ("GOST_generator/bin/f4_x.bin", "GOST_generator/bin/f4_y.bin",
           lambda x: utils.split_by_bit(x, 16), lambda x: utils.split_by_bit(x, 16), 2),
    "f5": ("GOST_generator/bin/f5_x.bin", "GOST_generator/bin/f5_y.bin",
           lambda x: utils.split_by_bit(x, 16), lambda x: utils.split_by_bit(x, 16), 2),
    "f6": ("GOST_generator/bin/f6_x.bin", "GOST_generator/bin/f6_y.bin",
           lambda x: utils.split_by_bit(x, 16), lambda x: utils.split_by_bit(x, 16), 2),
    "f7": ("GOST_generator/bin/f7_x.bin", "GOST_generator/bin/f7_y.bin",
           lambda x: utils.split_by_bit(x, 16), lambda x: utils.split_by_bit(x, 16), 2),
}


def get_test_data(model: str):
    return utils.get_primitive(models[model])


def experiment_std(input_data, output_data, n_input, n_classes, training_epochs=15000, display_step=1000):
    x, y, _, _ = networks.init_multilayer_network(input_data, output_data, n_input, n_classes,
                                                  training_epochs=training_epochs, display_step=display_step)

    x, y, _, _ = networks.init_multilayer_network(input_data, output_data, n_input, [4], n_classes, 1,
                                                  training_epochs=training_epochs, display_step=display_step)

    x, y, _, _ = networks.init_multilayer_network(input_data, output_data, n_input, [8], n_classes, 1,
                                                  training_epochs=training_epochs, display_step=display_step)

    x, y, _, _ = networks.init_multilayer_network(input_data, output_data, n_input, [16], n_classes, 1,
                                                  training_epochs=training_epochs, display_step=display_step)

    x, y, _, _ = networks.init_multilayer_network(input_data, output_data, n_input, [32], n_classes, 1,
                                                  training_epochs=training_epochs, display_step=display_step)

    x, y, _, _ = networks.init_multilayer_network(input_data, output_data, n_input, [32, 32], n_classes, 2,
                                                  training_epochs=training_epochs, display_step=display_step)


def experiment_changeable_0l(input_data, output_data, n_input, n_classes, model_name,
                             training_epochs_min=5000, training_epochs_max=30000, display_step=20000):
    accurancy = []
    loss = []
    train = []
    for i in range(training_epochs_min, training_epochs_max, 5000):
        x, y, _, _ = networks.init_one_layer_network(input_data, output_data, n_input, n_classes,
                                                     training_epochs=i, display_step=display_step)
        accurancy.append(n_classes - x)
        loss.append(y)
        train.append(i)
    create_graph(train, accurancy, ("Number of Training Epochs", "Accurancy", "Model " + model_name),
                 "results/acc_" + model_name + "_0.png")
    create_graph(train, loss, ("Number of Training Epochs", "Accurancy", "Model " + model_name),
                 "results/los" + model_name + "_0.png")


def experiment_changeable_1l(input_data, output_data, n_input, n_classes, left_b, right_b, model_name,
                             training_epochs=15000, display_step=20000):
    accurancy = []
    loss = []
    n_hidden = []
    for i in range(left_b, right_b + 1, 4):
        x, y, _, _ = networks.init_multilayer_network(input_data, output_data, n_input, [i], n_classes, 1,
                                                      training_epochs=training_epochs, display_step=display_step)
        accurancy.append(n_classes - x)
        loss.append(y)
        n_hidden.append(i)
    create_graph(n_hidden, accurancy, ("Number of neurons on hidden layer", "Accurancy", "Model " + model_name),
                 "results/acc_" + model_name + "_1.png", range(left_b, right_b + 1, 4))
    create_graph(n_hidden, loss, ("Number of neurons on hidden layer", "Accurancy", "Model " + model_name),
                 "results/los" + model_name + "_1.png", range(left_b, right_b + 1, 4))


def experiment_x(input_data, output_data, n_input, n_layers, n_classes, training_epochs=15000, display_step=20000):
    left_blocks = []
    right_blocks = []
    output_blocks = []
    real_results = []
    predict_results = []
    total_accuracy = 0
    for i in range(0, 8):
        left_blocks.append(input_data[:, 4 * i: 4 * i + 4])
        right_blocks.append(input_data[:, 32 + 4 * i: 32 + 4 * i + 4])
        output_blocks.append(output_data[:, 4 * i: 4 * i + 4])

    for i in range(0, 8):
        z, _, x, y = networks.init_multilayer_network(np.concatenate((left_blocks[i], right_blocks[i]), axis=1),
                                                      output_blocks[i], n_input, [n_layers],
                                                      n_classes, 1, training_epochs=training_epochs,
                                                      display_step=display_step)
        predict_results.append(x)
        real_results.append(y)
        total_accuracy += z
    print(total_accuracy)
    return total_accuracy


def experiment_changeable_x(input_data, output_data, n_input, n_classes, left_b, right_b,
                            training_epochs=15000, display_step=20000):
    accurancy = []
    n_hidden = []
    for i in range(left_b, right_b + 1, 4):
        x = experiment_x(input_data, output_data, n_input, i, n_classes)
        accurancy.append(n_classes * 8 - x)
        n_hidden.append(i)
    create_graph(n_hidden, accurancy, ("Number of neurons on hidden layer", "Accurancy", "GOST"),
                 "results/acc_x_1.png", range(left_b, right_b + 1, 4))


def experiment_changeable_2l(input_data, output_data, n_input, n_classes, left_b, right_b, model_name,
                             training_epochs=15000, display_step=20000):
    accurancy = []
    tikcs = []
    loss = []
    n_hidden = []
    z = 0
    for i in (8, 16, 32):
        for j in (8, 16, 32):
            x, y, _, _ = networks.init_multilayer_network(input_data, output_data, n_input, [i, j], n_classes, 2,
                                                          training_epochs=training_epochs, display_step=display_step)
            accurancy.append(n_classes - x)
            loss.append(y)
            n_hidden.append(z)
            tikcs.append(str((i, j)))
            z += 1
    create_graph(n_hidden, accurancy, ("Number of neurons on hidden layer", "Accurancy", "Model " + model_name),
                 "results/acc_" + model_name + "_2.png", tikcs)
    create_graph(n_hidden, loss, ("Number of neurons on hidden layer", "Accurancy", "Model " + model_name),
                 "results/los_" + model_name + "_2.png", tikcs)


def create_graph(x, y, legend: list, filename: str, ticks=None):
    plt.plot(x, y)
    if ticks is not None:
        plt.xticks(x, ticks)

    plt.xlabel(legend[0])
    plt.ylabel(legend[1])

    plt.title(legend[2])

    plt.savefig(filename)
    plt.close()


if __name__ == "__main__":
    # Network Parameters
    n_input = 16
    n_classes = 16

    model = "f7"
    input_data, output_data = get_test_data(model=model)

    #experiment_changeable_0l(input_data, output_data, n_input, n_classes, model)
    experiment_changeable_1l(input_data, output_data, n_input, n_classes, 16, 32, model, training_epochs=50000)
    #experiment_changeable_2l(input_data, output_data, n_input, n_classes, 8, 32, model)

    #
    # input_data, output_data = get_test_values(model="g1")
    # print("Experiment g1\n\n")
    # experiment_changeable_0l(input_data, output_data, n_input, n_classes, "g1")
    #
    # input_data, output_data = get_test_values(model="g2")
    # print("Experiment g2\n\n")
    # experiment_changeable_0l(input_data, output_data, n_input, n_classes, "g2")

    # n_input = 8
    # n_classes = 4
    # input_data, output_data = get_test_values1([64, 32])
    # print("Experiment g3\n\n")
    # # experiment_x(input_data, output_data, 8, 4)
    # experiment_changeable_x(input_data, output_data, n_input, n_classes, 8, 32)
    # experiment_changeable_0l(input_data, output_data, n_input, n_classes, "g3")
    # experiment_changeable_1l(input_data, output_data, n_input, n_classes, 8, 32, "g3")
    # experiment_changeable_2l(input_data, output_data, n_input, n_classes, 8, 32, "g3")
