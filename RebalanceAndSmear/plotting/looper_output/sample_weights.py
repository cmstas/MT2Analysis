list_of_ext_weights = {153: 9.26780033112, 154: 0.728529989719, 155: -1, 156: 0.116499997675, 157: 0.01523800008, 158: 0.00626630010083}
list_of_non_ext_weights = {153: 20.4099998474, 154: 1.72950005531, 155: 0.437070012093, 156: 0.248830005527, 157: 0.0301949996501, 158: 0.0126729998738}

def get_weight(sample_id, is_ext):
    if is_ext:
        return list_of_ext_weights[int(sample_id)]
    else:
        return list_of_non_ext_weights[int(sample_id)]

def get_total_weight(sample_id):
  w_ext = list_of_ext_weights[int(sample_id)]
  w_next = list_of_non_ext_weights[int(sample_id)]
  w = 1. / ((1./w_ext) + (1./w_next))
  return w
