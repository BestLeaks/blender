/*
 * Copyright 2011-2018 Blender Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "graph/node.h"
#include "render/attribute.h"
#include "render/procedural.h"
#include "util/util_transform.h"
#include "util/util_vector.h"

//#ifdef WITH_ALEMBIC

#include <Alembic/AbcCoreFactory/All.h>
#include <Alembic/AbcGeom/All.h>

using namespace Alembic::AbcGeom;

CCL_NAMESPACE_BEGIN

class Geometry;
class Object;
class Shader;

class AlembicObject : public Node {
 public:
  NODE_DECLARE

  AlembicObject();
  ~AlembicObject();

  NODE_SOCKET_API(ustring, path)
  NODE_SOCKET_API_ARRAY(array<Node *>, used_shaders)

  void set_object(Object *object);
  Object *get_object();

  void load_all_data(const IPolyMeshSchema &schema);

  bool has_data_loaded() const;

  // TODO : this is only for Meshes at the moment
  // TODO : handle attributes as well

  struct AttributeData {
    AttributeStandard std;
    AttributeElement element;
    TypeDesc type_desc;
    ustring name;
    array<char> data;
  };

  struct DataCache {
    bool dirty = false;
    array<float3> vertices{};
    array<int3> triangles{};
    array<int3> triangles_loops{};

    vector<AttributeData> attributes{};
  };

  DataCache &get_frame_data(int index);

  IObject iobject;
  Transform xform;

 private:
  Object *object = nullptr;
  Geometry *geometry = nullptr;

  // runtime data
  bool data_loaded = false;

  vector<DataCache> frame_data;

  void read_attribute(const ICompoundProperty &arb_geom_params,
                      const ISampleSelector &iss,
                      const ustring &attr_name,
                      DataCache &data_cache);
};

class AlembicProcedural : public Procedural {
 public:
  NODE_DECLARE

  AlembicProcedural();
  ~AlembicProcedural();
  void generate(Scene *scene);

  NODE_SOCKET_API(bool, use_motion_blur)
  NODE_SOCKET_API(ustring, filepath)
  NODE_SOCKET_API(float, frame)
  NODE_SOCKET_API(float, frame_rate)

  array<AlembicObject *> objects;  // todo : Node::set

 private:
  IArchive archive;
  bool objects_loaded = false;

  void load_objects();

  void read_mesh(Scene *scene,
                 AlembicObject *abc_object,
                 Transform xform,
                 IPolyMesh &mesh,
                 Abc::chrono_t frame_time);

  void read_curves(Scene *scene,
                   AlembicObject *abc_object,
                   Transform xform,
                   ICurves &curves,
                   Abc::chrono_t frame_time);
};

CCL_NAMESPACE_END