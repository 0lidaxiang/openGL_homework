﻿#include "GUA_OM.h"

using namespace std;
namespace OMT
{
	/*======================================================================*/
	Model::Model()
	{
		request_vertex_status();
		request_edge_status();
		request_face_status();
	}
	Model::~Model()
	{
		release_vertex_status();
		release_edge_status();
		release_face_status();
	}
}
/*======================================================================*/
namespace OMP
{
	Model::Model()
	{
		Mesh.request_vertex_status();
		Mesh.request_edge_status();
		Mesh.request_face_status();
	}
	Model::~Model()
	{
		Mesh.release_vertex_status();
		Mesh.release_edge_status();
		Mesh.release_face_status();
	}
	/*======================================================================*/
	bool Model::ReadFile(std::string _fileName)
	{
		bool isRead = false;
		OpenMesh::IO::Options opt;
		if ( OpenMesh::IO::read_mesh(Mesh, _fileName, opt) )
		{
			//read mesh from filename OK!
			isRead = true;
		}
		if(isRead)
		{
			// If the file did not provide vertex normals and mesh has vertex normal ,then calculate them
			if (!opt.check( OpenMesh::IO::Options::VertexNormal ) && Mesh.has_vertex_normals())
			{
				Mesh.update_normals();
			}
		}
		return isRead;
	}
	bool Model::SaveFile(std::string _fileName)
	{
		bool isSave = false;
		OpenMesh::IO::Options opt;
		if ( OpenMesh::IO::write_mesh(Mesh, _fileName, opt) )
		{
			//read mesh from filename OK!
			isSave = true;
		}
		return isSave;
	}
	/*======================================================================*/
	void Model::Render_solid()
	{
		FIter f_it;
		FVIter	fv_it;
		glPushAttrib(GL_LIGHTING_BIT);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glEnable(GL_DEPTH_TEST);
		glPolygonOffset(2.0, 2.0);
		glBegin(GL_POLYGON);
		//glColor4f(1.0, 0.5, 1.0, 0.5);
		for (f_it = Mesh.faces_begin(); f_it != Mesh.faces_end(); ++f_it) 
		{
			for (fv_it = Mesh.fv_iter( f_it ); fv_it; ++fv_it)
			{						
				glNormal3dv(Mesh.normal(fv_it.handle()).data());
				glVertex3dv(Mesh.point(fv_it.handle()).data());
			}
		}
		glEnd();		
		glDisable(GL_POLYGON_OFFSET_FILL);
	}
	void Model::Render_wireframe()
	{
		MyMesh::HalfedgeHandle _hedge;
		EIter e_it=Mesh.edges_begin();

		glDisable(GL_LIGHTING);
		glEnable(GL_DEPTH_TEST);
		glColor3f(0.0, 0.0, 0.0);
		glLineWidth(1);
		glBegin(GL_LINES);
		for(e_it=Mesh.edges_begin(); e_it != Mesh.edges_end(); ++e_it)
		{
			_hedge = Mesh.halfedge_handle(e_it.handle(),1);

			glVertex3dv(Mesh.point(Mesh.from_vertex_handle(_hedge)).data());
			glVertex3dv(Mesh.point(Mesh.to_vertex_handle(_hedge)).data());			
		}
		glEnd();
		glEnable(GL_LIGHTING);
	}
	/*======================================================================*/
	void Model::RenderSpecifiedPoint()
	{
		glPushAttrib(GL_LIGHTING_BIT);
		glDisable(GL_LIGHTING);
		glEnable(GL_DEPTH_TEST);
		glPointSize(5.0f);
		glBegin(GL_POINTS);
		vector<sp_p>::iterator p_itr = sp_p_list.begin();
		for (p_itr; p_itr != sp_p_list.end(); ++p_itr)
		{
			glColor3f(p_itr->r, p_itr->g, p_itr->b);
			glVertex3dv(p_itr->pt.data());
		}
		glEnd();
		glEnable(GL_LIGHTING);
		glDisable(GL_POLYGON_OFFSET_FILL);
	}
	void Model::RenderSpecifiedVertex()
	{
		glPushAttrib(GL_LIGHTING_BIT);
		glDisable(GL_LIGHTING);
		glEnable(GL_DEPTH_TEST);
		glPointSize(5.0f);
		glBegin(GL_POINTS);
		vector< sp_v >::iterator v_itr = sp_v_list.begin();
		for (v_itr; v_itr != sp_v_list.end(); ++v_itr)
		{
			glColor3f(v_itr->r, v_itr->g, v_itr->b);
			glVertex3dv(Mesh.point(v_itr->vh).data());
		}
		glEnd();
		glEnable(GL_LIGHTING);
		glDisable(GL_POLYGON_OFFSET_FILL);
	}
	void Model::RenderSpecifiedFace()
	{
		glDisable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		glPushAttrib(GL_LIGHTING_BIT);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(0.5, 1.0);
		glDisable(GL_LIGHTING);
		glEnable(GL_DEPTH_TEST);
		glBegin(GL_QUADS);
		FVIter fv_itr;
		vector< sp_f >::iterator f_itr;
		for (f_itr = sp_f_list.begin(); f_itr != sp_f_list.end(); ++f_itr)
		{
			glColor3f(f_itr->r, f_itr->g, f_itr->b);
			for (fv_itr=Mesh.fv_iter(f_itr->fh); fv_itr; ++fv_itr)
			{						
				glNormal3dv(Mesh.normal(fv_itr.handle()).data());
				glVertex3dv(Mesh.point(fv_itr.handle()).data());
			}
		}
		glEnd();		
		glEnable(GL_LIGHTING);
		glDisable(GL_POLYGON_OFFSET_FILL);
		glPolygonMode(GL_FRONT,GL_FILL);
		glEnable(GL_CULL_FACE);
	}

	/*======================================================================*/
	void Model::add_sp_p(Point   _p, float _r, float _g, float _b)
	{
		sp_p input_data;
		input_data.pt = _p;
		input_data.r = _r;
		input_data.g = _g;
		input_data.b = _b;
		sp_p_list.push_back(input_data);
	}
	void Model::add_sp_v(VHandle _v, float _r, float _g, float _b)
	{
		sp_v input_data;
		input_data.vh = _v;
		input_data.r = _r;
		input_data.g = _g;
		input_data.b = _b;
		sp_v_list.push_back(input_data);
	}
	void Model::add_sp_f(FHandle _f, float _r, float _g, float _b)
	{
		sp_f input_data;
		input_data.fh = _f;
		input_data.r = _r;
		input_data.g = _g;
		input_data.b = _b;
		sp_f_list.push_back(input_data);
	}
	void Model::clear_sp_p()
	{
		sp_p_list.clear();
	}
	void Model::clear_sp_v()
	{
		sp_v_list.clear();
	}
	void Model::clear_sp_f()
	{
		sp_f_list.clear();
	}
	/*======================================================================*/
	VHandle Model::addVertex(Point _p)
	{
		int find_result = findVertex(_p);
		if (find_result != -1)
		{
			return Mesh.vertex_handle(find_result);
		}
		else
		{
			return Mesh.add_vertex(_p);
		}
	}
	FHandle Model::addFace(VHandle _v0, VHandle _v1, VHandle _v2, VHandle _v3)
	{
		vector<VHandle> face_vhandles;

		face_vhandles.clear();
		face_vhandles.push_back(_v0);
		face_vhandles.push_back(_v1);
		face_vhandles.push_back(_v2);
		face_vhandles.push_back(_v3);
		return Mesh.add_face(face_vhandles);
	}
	void Model::deleteFace(FHandle _f)
	{
		Mesh.delete_face(_f);
		Mesh.garbage_collection();
	}
	void Model::deleteFace(VHandle _v0, VHandle _v1, VHandle _v2, VHandle _v3)
	{
		/* 
		v1				v0
		*<--------------*
		|				|
		|				|
		|				|
		|		f		|
		|				|
		|				|
		|				|
		* --------------*
		v2				v3
		*/

		HEHandle v0v1 = Mesh.find_halfedge(_v0, _v1);
		if (v0v1.is_valid())
		{
			FHandle fh = Mesh.face_handle(v0v1);
			if (fh.is_valid())
			{
				Mesh.delete_face(fh);
				Mesh.garbage_collection();
			}
		}
	}
	/*======================================================================*/
	bool Model::IsVertexVertex( VHandle _vj, VHandle _vi)
	{
		for( VVIter vvit = Mesh.vv_iter(_vi) ; vvit ; ++vvit )
			if( vvit.handle() == _vj )
				return true;
		return false;
	}
	/*======================================================================*/
	int Model::quad_subdivision1(int _face_id)
	{
		/*----------------------------------------------------------------------*/
		//這段是為了解決index問題
		VHandle vq, vw, vt, vr;
		vq	 = addVertex(Point(0, 0, 100));
		vw	 = addVertex(Point(1, 0, 100));
		vt	 = addVertex(Point(1, 1, 100));
		vr	 = addVertex(Point(0, 1, 100));
		addFace(vq, vw, vt, vr);
		/*----------------------------------------------------------------------*/
		/*收集需要subdivision的face*/
		//初始table
		bool *table = new bool[Mesh.n_faces()];
		for (unsigned i = 0; i < Mesh.n_faces(); i++)
		{
			table[i] = false;
		}

		vector< FHandle > candidate_faces, last_found_face;
		last_found_face.push_back(Mesh.face_handle(_face_id));
		table[_face_id] = true;

		while(last_found_face.size() != 0)
		{
			vector< FHandle > new_found_faces;
			for (vector< FHandle >::iterator crnt_f = last_found_face.begin(); crnt_f != last_found_face.end(); ++crnt_f)
			{
				for (FFIter ff_itr = Mesh.ff_iter(*crnt_f); ff_itr; ++ff_itr)
				{
					if (table[ff_itr.handle().idx()] != true)
					{
						new_found_faces.push_back(ff_itr.handle());
						table[ff_itr.handle().idx()] = true;
					}
				}
			}
			for (vector< FHandle >::iterator f_itr = last_found_face.begin(); f_itr != last_found_face.end(); ++f_itr)
			{
				candidate_faces.push_back(*f_itr);
			}
			last_found_face.assign(new_found_faces.begin(), new_found_faces.end());
		}
		delete table;
		/*----------------------------------------------------------------------*/
		/*對candidate faces做subdivision*/
		/*
			v0		vd		v3
			*-------*-------*
			|		|		|	 
			|		|		|
			|	  ve|		|
		  va*-------*-------*vc
			|		|		|
			|		|		|
			|		|		|
			*-------*-------*
			v1		vb		v2		
		*/
		for (vector< FHandle >::iterator face_itr = candidate_faces.begin(); face_itr != candidate_faces.end(); ++face_itr)
		{
			VHandle v[4], va, vb, vc, vd, ve;
			FVIter fv_itr = Mesh.fv_iter(*face_itr);
			for (int i = 0; fv_itr; ++fv_itr)
			{
				v[i++] = fv_itr.handle();
			}

			deleteFace(v[0], v[1], v[2], v[3]);
			va	 = addVertex((Mesh.point(v[0])+Mesh.point(v[1]))/2);
			vb	 = addVertex((Mesh.point(v[1])+Mesh.point(v[2]))/2);
			vc	 = addVertex((Mesh.point(v[2])+Mesh.point(v[3]))/2);
			vd	 = addVertex((Mesh.point(v[3])+Mesh.point(v[0]))/2);
			ve	 = addVertex((Mesh.point(v[0])+Mesh.point(v[1])+Mesh.point(v[2])+Mesh.point(v[3]))/4);
			addFace(vd, v[0], va, ve);
			addFace(va, v[1], vb, ve);
			addFace(vb, v[2], vc, ve);
			addFace(vc, v[3], vd, ve);
		}
		/*----------------------------------------------------------------------*/
		deleteFace(vq, vw, vt, vr);//這行只是為了解決index問題
		/*----------------------------------------------------------------------*/
		return 0;
	}
	int Model::quad_subdivision2(int _face_id)
	{
		/*----------------------------------------------------------------------*/
		//這段是為了解決index問題
		VHandle vq, vw, vt, vr;
		vq	 = addVertex(Point(0, 0, 100));
		vw	 = addVertex(Point(1, 0, 100));
		vt	 = addVertex(Point(1, 1, 100));
		vr	 = addVertex(Point(0, 1, 100));
		addFace(vq, vw, vt, vr);
		/*----------------------------------------------------------------------*/
		/*收集需要subdivision的face*/
		//初始table
		bool *table = new bool[Mesh.n_faces()];
		for (unsigned i = 0; i < Mesh.n_faces(); i++)
		{
			table[i] = false;
		}

		vector< FHandle > candidate_faces, last_found_face;
		last_found_face.push_back(Mesh.face_handle(_face_id));
		table[_face_id] = true;

		while(last_found_face.size() != 0)
		{
			vector< FHandle > new_found_faces;
			for (vector< FHandle >::iterator crnt_f = last_found_face.begin(); crnt_f != last_found_face.end(); ++crnt_f)
			{
				for (FFIter ff_itr = Mesh.ff_iter(*crnt_f); ff_itr; ++ff_itr)
				{
					if (table[ff_itr.handle().idx()] != true)
					{
						new_found_faces.push_back(ff_itr.handle());
						table[ff_itr.handle().idx()] = true;
					}
				}
			}
			for (vector< FHandle >::iterator f_itr = last_found_face.begin(); f_itr != last_found_face.end(); ++f_itr)
			{
				candidate_faces.push_back(*f_itr);
			}
			last_found_face.assign(new_found_faces.begin(), new_found_faces.end());
		}
		delete table;
		/*----------------------------------------------------------------------*/
		/*對candidate faces做subdivision*/
		/*
			v0		vh		vg		v3
			*-------*-------*-------*
			|		|		|		|
			|		|		|		|
			|	  vi|	  vl|		|
		 va *-------*-------*-------*vf
			|		|		|		|
			|		|		|		|
			|	  vj|	  vk|		|
		 vb *-------*-------*-------*ve
			|		|		|		|
			|		|		|		|
			|		|		|		|
			*-------*-------*-------*
			v1		vc		vd		v2		
		*/
		for (vector< FHandle >::iterator face_itr = candidate_faces.begin(); face_itr != candidate_faces.end(); ++face_itr)
		{
			VHandle v[4], va, vb, vc, vd, ve, vf, vg, vh, vi, vj, vk, vl;
			FVIter fv_itr = Mesh.fv_iter(*face_itr);
			for (int i = 0; fv_itr; ++fv_itr)
			{
				v[i++] = fv_itr.handle();
			}

			deleteFace(v[0], v[1], v[2], v[3]);
			va	 = addVertex((Mesh.point(v[0])*2+Mesh.point(v[1])  )/3);
			vb	 = addVertex((Mesh.point(v[0])  +Mesh.point(v[1])*2)/3);
			vc	 = addVertex((Mesh.point(v[1])*2+Mesh.point(v[2])  )/3);
			vd	 = addVertex((Mesh.point(v[1])  +Mesh.point(v[2])*2)/3);
			ve	 = addVertex((Mesh.point(v[2])*2+Mesh.point(v[3])  )/3);
			vf	 = addVertex((Mesh.point(v[2])  +Mesh.point(v[3])*2)/3);
			vg	 = addVertex((Mesh.point(v[3])*2+Mesh.point(v[0])  )/3);
			vh	 = addVertex((Mesh.point(v[3])  +Mesh.point(v[0])*2)/3);

			vi	 = addVertex((Mesh.point(vh)*2+Mesh.point(vc)  )/3);
			vj	 = addVertex((Mesh.point(vh)  +Mesh.point(vc)*2)/3);
			vk	 = addVertex((Mesh.point(vd)*2+Mesh.point(vg)  )/3);
			vl	 = addVertex((Mesh.point(vd)  +Mesh.point(vg)*2)/3);
			addFace(v[0], va, vi, vh);
			addFace(va, vb, vj, vi);
			addFace(vb, v[1], vc, vj);
			addFace(vc, vd, vk, vj);
			addFace(vd, v[2], ve, vk);
			addFace(ve, vf, vl, vk);
			addFace(vf, v[3], vg, vl);
			addFace(vg, vh, vi, vl);
			addFace(vi, vj, vk, vl);
		}
		/*----------------------------------------------------------------------*/
		deleteFace(vq, vw, vt, vr);//這行只是為了解決index問題
		/*----------------------------------------------------------------------*/
		return 0;
	}
	/*======================================================================*/
	int Model::findVertex(Point _p)
	{
		for( VIter v_itr = Mesh.vertices_begin(); v_itr != Mesh.vertices_end(); ++v_itr)
			if( Mesh.point(v_itr) == _p )
				return v_itr.handle().idx();
		return -1;
	}
	/*======================================================================*/
};
/*======================================================================*/
void Tri_Mesh::Render_Solid()
{
		FIter f_it;
		FVIter	fv_it;
		//glPushAttrib(GL_LIGHTING_BIT);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glEnable(GL_LIGHTING);
		glPolygonOffset(2.0, 2.0);
		glBegin(GL_TRIANGLES);
		glColor4f(0.81, 0.74, 0.33, 0.3);
		for (f_it = faces_begin(); f_it != faces_end(); ++f_it) 
		{
			for (fv_it = fv_iter( f_it ); fv_it; ++fv_it)
			{						
				glNormal3dv(normal(fv_it.handle()).data());
				glVertex3dv(point(fv_it.handle()).data());
			}
		}
		glEnd();	
		
		glDisable(GL_POLYGON_OFFSET_FILL);
}

GLuint loadTexture(Image* image) {
	GLuint textureId;
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexImage2D(GL_TEXTURE_2D,
		0,
		GL_RGB,
		image->width, image->height,
		0,
		GL_RGB,
		GL_UNSIGNED_BYTE,
		image->pixels);
	return textureId;
}

void Tri_Mesh::Render_SolidWireframe()
{
	FIter f_it;
	FVIter	fv_it;

	glDisable(GL_LIGHTING);
	glPushAttrib(GL_LIGHTING_BIT);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glEnable(GL_DEPTH_TEST);
	glPolygonOffset(2.0, 2.0);
	glBegin(GL_TRIANGLES);

		for (OMT::FIter f_it = faces_begin(); f_it != faces_end(); ++f_it)
		{
			if(status(f_it).selected())
			{
				//glColor4f(1.0, 0, 0, 1.0);
				for (fv_it = fv_iter(f_it); fv_it; ++fv_it)
				{
					Render_DrawTexture();
					glNormal3dv(normal(fv_it.handle()).data());
					glVertex3dv(point(fv_it.handle()).data()); 
					
				}
			}
			else 
			{
				glColor4f(1.0, 0.96, 0.49, 1.0);
				for (fv_it = fv_iter(f_it); fv_it; ++fv_it)
				{
					glNormal3dv(normal(fv_it.handle()).data());
					glVertex3dv(point(fv_it.handle()).data());
				}
			}
		}
	
	glEnd();

	glPushAttrib(GL_LIGHTING_BIT);
	glDisable(GL_LIGHTING);
	glLineWidth(1.0);
	glColor3f(0.0, 0.0, 0.0);
	glBegin(GL_LINES);
	for (OMT::EIter e_it = edges_begin(); e_it != edges_end(); ++e_it)
	{
		OMT::HEHandle _hedge = halfedge_handle(e_it.handle(), 1);

		OMT::Point curVertex = point(from_vertex_handle(_hedge));
		glVertex3dv(curVertex.data());

		curVertex = point(to_vertex_handle(_hedge));
		glVertex3dv(curVertex.data());
	}
	glEnd();
	glPopAttrib();
}

//GLuint _textureId;

vector<double> texCoord2dX;
vector<double> texCoord2dY;
vector <OMT::VHandle> boundaryVector;

void Tri_Mesh::Render_SelectFace()
{
	
	//delete vertex
	request_vertex_status();
	bool not_delete = false;
	//对newmesh里的未被选择点标记为“清除”
	for (VIter it = vertices_begin(); it != vertices_end(); ++it)
	{
		not_delete = false;

		for (OMT::VFIter vf_it = vf_iter(it); vf_it; ++vf_it)
		{
			if (status(vf_it).selected())
			{
				not_delete = true;
				break;
			}
		}
		if (!not_delete)
		{
			delete_vertex(it.handle(), false);
		}
	}
	//清除之前标记的点及面
	garbage_collection();

	/*cout << "n_faces(): " << n_faces() << endl;
	cout << "n_vertices(): " << n_vertices() << endl;*/

	//找到第一个外围点的handle值
	bool can_record = false;
	VHandle first_vertex;
	for (VIter it = vertices_begin(); it != vertices_end(); it++) 
	{
		if (is_boundary(it))
		{
				first_vertex = it.handle();
				can_record = true;
				cout << "first vertex handle:" << it.handle() << endl;
		}
		if (can_record)
		{
			break;
		}
	}

	//把外围点的handle值按照逆时针顺序存起来
	HHandle hh_init = halfedge_handle(first_vertex);
	HHandle hh = hh_init;
	boundaryVector.clear();
	boundaryVector.push_back(from_vertex_handle(hh));
	hh = next_halfedge_handle(hh);
	while (hh != hh_init) {
		boundaryVector.push_back(from_vertex_handle(hh));
		hh = next_halfedge_handle(hh);
	}
	cout << endl;

	//下面只是测试用的输出坐标，但这里的坐标已经可以填入glVertex3d了
	cout << "选中多边形的所有外围点的坐标(順時針):  " << endl;
	for (int i = 0; i< boundaryVector.size(); i++)
	{
		for (int j = 0; j < point(boundaryVector[i]).size(); j++)
		{
			cout << point(boundaryVector[i])[j] << ", ";
		}
		cout << "  ||   ";
	}
	cout << endl;
	
	//计算多边形顶点在贴图上对应的位置，以便填入glTexCoord2d
	double sum_distance = 0;
	double rate = 0;
	
	OMT::Vec3d p ;
	OMT::Vec3d q ;
	//计算多边形顶点在贴图上对应的位置，以便填入glTexCoord2d
	//计算多边形总的周长
	for (int i = 0; i < boundaryVector.size() ; i++)
	{
		if (i == (boundaryVector.size() - 1))
		{
			p = point(boundaryVector[i]);
			q = point(boundaryVector[0]);
		}
		else 
		{
			p = point(boundaryVector[i]);
			q = point(boundaryVector[i + 1]);
		}
		sum_distance = sum_distance + (p - q).length();
	}
	cout <<"sum_distance: "<<sum_distance << endl;

	//计算多边形顶点在贴图上对应的位置，以便填入glTexCoord2d
	double head_now_length = 0.0;
	//先计算第一个点的对应在贴图中原点
	texCoord2dX.clear();
	texCoord2dY.clear();

	texCoord2dX.push_back(0.0);
	texCoord2dY.push_back(0.0);

	cout << "Render_SelectFace : texCoord2dX[0]： " << texCoord2dX[0] << " , texCoord2dY[0]" 
		<< texCoord2dY[0] << "   " << endl;;

	//计算多边形每个边长的所占长度，并计算存储相对应的2d贴图中的位置
	for (int i = 0; i < (boundaryVector.size() - 1); i++)
	{
		p = point(boundaryVector[i]);
		q = point(boundaryVector[i + 1]);

		head_now_length = head_now_length + (double)(p - q).length();

		rate = head_now_length / sum_distance;
		double point_in_texture = rate * 4;
		
		if (point_in_texture < 1)
		{
			texCoord2dX.push_back(rate * 4.0);
			texCoord2dY.push_back(0.0);
		}
		else if (point_in_texture < 2)
		{
			texCoord2dX.push_back(1.0);
			texCoord2dY.push_back(rate * 4.0 - 1);
		}
		else if (point_in_texture < 3)
		{
			texCoord2dX.push_back(3 - rate * 4.0);
			texCoord2dY.push_back(1.0);
		}
		else if (point_in_texture < 4)
		{
			texCoord2dX.push_back(0.0);
			texCoord2dY.push_back(4 - rate * 4.0);
		}
		else 
		{
			cout << "Some error happend.The rate should not over 1 ." << endl;
		}
	}
}

GLuint _textureId;
void LoadBMPinitRendering()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Image* image = loadBMP("vtr1.bmp");
	_textureId = loadTexture(image);
	delete image;
}
//vector<auto> newmeshNormal;
void Tri_Mesh::Render_DrawTexture()
{
	//cout << "Render_DrawTexture   texCoord2d.size :" << texCoord2dX.size() << endl;
	//cout << "Render_DrawTexture  boundaryVector.size :" << boundaryVector.size() << endl;
	if (boundaryVector.size() > 2)
	{
		//使用point函数，现在已经确定了texture需要的对应3D坐标，只要load图然后贴上去就ok
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, _textureId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		for (int i = 0; i < boundaryVector.size(); i++)
		{
			cout << "绘图boundaryVector： ";
			for (int j = 0; j < point(boundaryVector[i]).size(); j++)
			{
				cout << point(boundaryVector[i])[j] << ", ";
			}
			cout << endl;
		}

		for (int i = 0; i < texCoord2dX.size(); i++)
		{
			cout << "绘图texCoord2dX： ";
			cout << texCoord2dX[i] << ", ";
				cout << texCoord2dY[i] <<  endl;
		}
		cout <<"\n" << endl;
		
		glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, _textureId);
		glBegin(GL_TRIANGLES);
		glNormal3f(1, 0, 1);
		for (int i = 0; i < boundaryVector.size(); i++)
		{
			glTexCoord2d(texCoord2dX[i], texCoord2dY[i]);
			glVertex3d(point(boundaryVector[i])[0], point(boundaryVector[i])[1], point(boundaryVector[i])[2]);
		}
		glEnd();
		glPopMatrix();
	}
	
}

void Tri_Mesh::Render_Wireframe()
{
	//glPushAttrib(GL_LIGHTING_BIT);	
	glDisable(GL_LIGHTING);
	glLineWidth(1.0);
	
	glColor3f(0.0, 0.0, 0.0);	

	glBegin(GL_LINES);
	for(OMT::EIter e_it = edges_begin(); e_it != edges_end(); ++e_it)
	{
		OMT::HEHandle _hedge = halfedge_handle(e_it.handle(),1);

		OMT::Point curVertex  = point(from_vertex_handle(_hedge));
		glVertex3dv(curVertex.data());

		curVertex = point(to_vertex_handle(_hedge));
		glVertex3dv(curVertex.data());			
	}
	glEnd();
	
}

void Tri_Mesh::Render_Point()
{
	glPointSize ( 8.0 ) ;				  
	glColor3f( 1.0, 0.0, 0.0 ) ;
	glBegin(GL_POINTS);
	for (OMT::VIter v_it = vertices_begin() ; v_it != vertices_end() ; ++v_it)
	{
		  glVertex3dv(point(v_it).data());
	}
	glEnd();
}

void Tri_Mesh::SelectFace(GLdouble objX, GLdouble objY, GLdouble objZ)
{
	if (!has_vertex_normals())
	{
		request_vertex_normals();
		std::cout << "ERROR: Standard vertex property 'Normals' not available! AND I had added.\n";
	}

	FHandle fvh;//存储当前距离你点击的点最近的面的handle值
	double minArea = 1000;
	OMT::Vec3d q(objX, objY, objZ);

	//循环所有的面
	for (OMT::FIter f_it = faces_begin(); f_it != faces_end(); ++f_it)
	{
		double  a = -1, b = -1, c = -1, abc = -1, s1 = -1, s2 = -1, s3 = -1;//for search the face that you chicked on.
		double a0 = -1, b0 = -1, c0 = -1, s = -1, realS = -1;
		
		OMT::Vec3d verOfOuterTri[3];//选中三角形的三个顶点坐标
		int i = 0;

		//循环当前的面的所有点
		for (OMT::FVIter v_it = fv_iter(f_it); v_it; ++v_it)
		{
			OMT::Vec3d p = point(v_it.handle());

			verOfOuterTri[i] = p;
			i++;

			// Calculate the three side length of inside triangle
			if (a < 0)
			{
				a = (p - q).length();
			}
			else if (b < 0)
			{
				b = (p - q).length();
			}
			else if (c < 0)
			{
				c = (p - q).length();
			}
		}

		// Calculate the side length of outer triangle
		a0 = (verOfOuterTri[0] - verOfOuterTri[1]).length();
		b0 = (verOfOuterTri[1] - verOfOuterTri[2]).length();
		c0 = (verOfOuterTri[2] - verOfOuterTri[0]).length();

		//Calculate the area of real outer triangles in this face
		abc = (a + b + c) / 2;
		realS = sqrt(abc * (abc - a) * (abc - b) * (abc - c));

		//Calculate the area of three triangles in this face,just your clicked
		abc = (a + b + a0) / 2;
		s1 = sqrt(abc * (abc - a) * (abc -b) * (abc - a0));
		abc = (b0 + b + c) / 2;
		s2 = sqrt(abc * (abc - b0) * (abc - b) * (abc - c));
		abc = (a + c0 + c) / 2;
		s3 = sqrt(abc * (abc - a) * (abc - c0) * (abc - c));
		//Calculate the area of this face(no-real area,just your clicked)
		s = s1 + s2 + s3;

		//cout<< "外围大三角形的所有顶点的坐标： " << verOfOuterTri[0]<<"  " << verOfOuterTri[1] << "  " << verOfOuterTri[2] << endl;
		/*std::cout << "this face area: " << realS << std::endl;
		std::cout << "this face area(user clicked): "<< s << std::endl;*/

		//Compare the area of this face with the min-area 
		//And reacord the  handle of smaller result face
		if ((s - realS) < minArea)
		{
			minArea = s - realS;
			fvh = f_it.handle();
		}
	}
	
	//给所点击的面标记为“被选中”
	if (!status(fvh).selected())
	{
		status(fvh).set_selected(true);
		/*newmeshNormal.push_back(normal(fvh));
		cout << "Normal size: " << newmeshNormal.size() << endl;
		cout << "Normal begin: " << newmeshNormal[0] << endl;*/
		//cout << "Normal end: " << newmeshNormal.end() << endl;
	}
	cout << "Normal size: " << normal(fvh) << endl;
	for (VertexIter v_it = vertices_begin();
		v_it != vertices_end(); ++v_it)
	{
		std::cout << "Vertex #" << *v_it << ": " << point(*v_it);
		set_point(*v_it, point(*v_it) +normal(*v_it));
		std::cout << " moved to " << point(*v_it) << std::endl;
	}
}

bool ReadFile(std::string _fileName,Tri_Mesh *_mesh)
{
	bool isRead = false;
	OpenMesh::IO::Options opt;
	if ( OpenMesh::IO::read_mesh(*_mesh, _fileName, opt) )
	{
		//read mesh from filename OK!
		isRead = true;
	}
	if(isRead)
	{
		// If the file did not provide vertex normals and mesh has vertex normal ,then calculate them
		if (!opt.check( OpenMesh::IO::Options::VertexNormal ) && _mesh->has_vertex_normals())
		{
			_mesh->update_normals();
		}
	}
	return isRead;
}

bool SaveFile(std::string _fileName, Tri_Mesh *_mesh)
{
	bool isSave = false;
	if (OpenMesh::IO::write_mesh(*_mesh, _fileName))
	{
		isSave = true;
	}
	return isSave;
}
