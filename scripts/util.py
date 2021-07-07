def strip_array(array: list, offset_index: int):
    new_array = list()
    if len(array) > offset_index:
        for index in range(offset_index, len(array)):
            new_array.append(array[index])
    return new_array