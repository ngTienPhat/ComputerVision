from __future__ import division, print_function, absolute_import
import re
import glob
import os.path as osp
import warnings

from ..dataset import ImageDataset


class CustomMSMT17(ImageDataset):

    _junk_pids = [0, -1]
    # data_dir = '/content/drive/My Drive/PROJECT/person_reid/data'
    data_dir = '/content/data'
    dataset_dir = osp.join(data_dir, 'msmt17')

    def __init__(self, root='', market1501_500k=False, **kwargs):
        self.root = osp.abspath(osp.expanduser(root))
        self.dataset_dir = osp.join(self.root, self.dataset_dir)

        # allow alternative directory structure
        self.data_dir = self.dataset_dir
        
        
        self.train_dir = osp.join(self.data_dir, 'bounding_box_train')
        self.query_dir = osp.join(self.data_dir, 'query')
        
        self.gallery_dir = osp.join(self.data_dir, 'bounding_box_test')
        

        required_files = [
            self.data_dir, self.train_dir, self.query_dir, self.gallery_dir
        ]

        self.check_before_run(required_files)

        train = self.process_dir(self.train_dir, relabel=True)
        print("Finish processing train data")

        query = self.process_dir(self.query_dir, relabel=False)
        print("Finish processing query data")

        gallery = self.process_dir(self.gallery_dir, relabel=False)
        print("Finish processing gallery data")
        
        super(CustomMSMT17, self).__init__(train, query, gallery, **kwargs)

    def process_dir(self, dir_path, relabel=False):
        img_paths = glob.glob(osp.join(dir_path, '*.jpg'))

        print(f"There are {len(img_paths)} in {dir_path.split('/')[-1]}")
        pattern = re.compile(r'([-\d]+)_c([-\d]+)')

        pid_container = set()
        for img_path in img_paths:
            img_filename = img_path.split('/')[-1]
            
            pid, camid, order = img_filename.split('.')[0].split('_')

            # pid, _ = map(int, pattern.search(img_path).groups())
            # if pid == -1:
            #     continue # junk images are just ignored
            pid_container.add(pid)
        pid2label = {pid: label for label, pid in enumerate(pid_container)}

        data = []
        for img_path in img_paths:
            img_filename = img_path.split('/')[-1]
            
            pid, camid, order = img_filename.split('.')[0].split('_')
            # pid, camid = map(int, pattern.search(img_path).groups())
            # if pid == -1:
            #     continue # junk images are just ignored
            camid = int(camid[1:])
            camid -= 1 # index starts from 0
            if relabel:
                pid = pid2label[pid]
            data.append((img_path, int(pid), int(camid)))

        print(f"n_samples: {len(data)}, example: ")
        if len(data) > 0:
            print(data[0])

        return data
